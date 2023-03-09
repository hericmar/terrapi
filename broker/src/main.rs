mod cache;
mod error;
mod model;
mod repo;
mod rest;
mod schema;
mod utils;

use std::path::PathBuf;
use actix_web::{App, HttpRequest, HttpServer, web};
use actix_web::cookie::time;
use actix_web::middleware::Logger;
use clap::Parser;
use dotenv;
use serde::Deserialize;
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

    let context = Context{
        config: config.clone(),
        db: create_conn_pool(&config.database_url),
    };

    HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(context.clone()))
            // .wrap(Logger::new("%a"))
            .wrap(Logger::default())
            .service(
                web::scope("/api/v1")
                    .service(
                        web::resource("/client")
                            .route(web::get().to(rest::list_clients))
                            .route(web::post().to(rest::create_client))
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
            )
            .service(actix_files::Files::new("/", config.static_root.clone()).index_file("index.html"))
    })
        .bind(("localhost", config.port))?
        .run()
        .await
}
