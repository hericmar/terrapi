mod rest;
mod cache;
mod error;
mod repo;

use actix_web::{App, HttpServer, web};
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
#[derive(Deserialize, Debug)]
struct Config {
    port: u16,
    database_url: String,
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "debug");
    env_logger::init();

    let args = Args::parse();

    if let Some(env_file) = args.env_file {
        dotenv::from_filename(env_file).ok();
    }

    let config = match envy::from_env::<Config>() {
        Ok(config) => { config }
        Err(err) => { panic!("cannot read config: {}", err); }
    };

    let context = Context{
        db: create_conn_pool(&config.database_url),
    };

    HttpServer::new(move || {
        App::new()
            .app_data(web::Data::new(context.clone()))
            .wrap(Logger::new("%a"))
    })
        .bind(("localhost", config.port))?
        .run()
        .await
}
