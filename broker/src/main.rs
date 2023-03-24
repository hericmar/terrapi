mod cache;
mod error;
mod model;
mod repo;
mod rest;
mod schema;

use std::fs;
use std::fs::OpenOptions;
use std::io::Write;
use std::path::{Path};
use std::sync::{Arc, Mutex};
use actix_cors::Cors;
use actix_web::{App, HttpServer, web};
use actix_web::middleware::Logger;
use clap::Parser;
use dotenv;
use serde::Deserialize;
use crate::cache::Cache;
use crate::repo::create_conn_pool;
use crate::rest::Context;

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    #[arg(short, long)]
    env_file: Option<String>
}

/// Deserialized from environment variables using `envy` crate.
#[derive(Clone, Deserialize, Debug)]
pub struct Config {
    pub admin_password: String,
    pub token_expiration: u64,
    pub port: u16,
    pub base_url: String,
    pub static_root: String,
    pub database_url: String,
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "debug");
    env_logger::init();

    let args = Args::parse();

    if let Some(env_file) = args.env_file {
        match dotenv::from_filename(&env_file) {
            Ok(_) => {}
            Err(_) => panic!("cannot read config from: {}", env_file)
        };
    }

    let config = match envy::from_env::<Config>() {
        Ok(config) => { config }
        Err(err) => { panic!("cannot read config: {}", err); }
    };

    {
        // set API URL
        let index = Path::new(&config.static_root.clone()).join("index.html");
        let contents = fs::read_to_string(&index)?;
        let new = contents.replace(
            "const API_URL = 'http://localhost:8091'",
            &format!("const API_URL = '{}'", &config.base_url)
        );

        let mut file = OpenOptions::new().write(true).truncate(true).open(&index)?;
        file.write(new.as_bytes())?;
    }

    let listen_port = config.port;
    let cache = Arc::new(Mutex::new(Cache::new()));

    // create web server
    HttpServer::new(move || {
        let context = Context{
            cache: cache.clone(),
            config: config.clone(),
            db: create_conn_pool(&config.database_url),
        };

        // replaced by Cors::permissive()
        /*
        let cors = Cors::default()
            // todo -> move this origin to config
            .allowed_origin("http://127.0.0.1:3000")
            .allowed_origin(&config.base_url.clone());
         */

        App::new()
            .app_data(web::Data::new(context))
            .wrap(Logger::default())
            // .wrap(cors)
            .wrap(Cors::permissive())
            .service(
                web::scope("/api/v1")
                    .service(
                        web::resource("/client")
                            .route(web::get().to(rest::list_clients))
                            .route(web::post().to(rest::create_client))
                    )
                    .service(
                        web::resource("/client/{client_id}/name")
                            .route(web::put().to(rest::rename_client))
                    )
                    .service(
                        web::resource("/client/preview")
                            .route(web::get().to(rest::list_client_preview))
                    )
                    .service(
                        web::resource("/client/{client_id}")
                            .route(web::get().to(rest::get_client))
                            .route(web::delete().to(rest::delete_client))
                    )
                    .service(
                        web::resource("/config")
                            .route(web::get().to(rest::list_configs))
                            .route(web::put().to(rest::put_config))
                    )
                    .service(
                        web::resource("/config/{client_id}")
                        .route(web::get().to(rest::get_config))
                    )
                    .service(
                        web::resource("/record")
                            .route(web::post().to(rest::post_records))
                    )
                    .service(
                        web::resource("/record/{client_id}")
                            .route(web::get().to(rest::get_records))
                    )
                    .service(web::resource("/login")
                        .route(web::post().to(rest::login))
                    )
                    .service(web::resource("/logout")
                        .route(web::post().to(rest::logout))
                )
            )
            .service(actix_files::Files::new("/", config.static_root.clone())
                .index_file("index.html")
            )
    })
        .bind(("localhost", listen_port))?
        .run()
        .await
}
