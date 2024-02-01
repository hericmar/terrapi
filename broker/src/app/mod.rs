pub mod publisher;
pub mod hello;

use actix_web::{App, HttpRequest, HttpServer, web};
use actix_web::dev::Server;
use actix_web::web::{Data, service};
use serde::Deserialize;
use crate::db;
use crate::db::DbPool;

/// Deserialized from environment variables using `envy` crate.
#[derive(Clone, Deserialize, Debug)]
pub struct Config {
    pub admin_password: String,
    pub bind_address: String,
    pub base_url: String,
    pub database_url: String,
}

pub struct AppState {
    pub db: DbPool,
}

async fn index(_ctx: Data<AppState>, _req: HttpRequest) -> &'static str {
    "Hello world!"
}

fn routes(app: &mut web::ServiceConfig) {
    app
        .route("/", web::get().to(index))
        .service(
            web::scope("/api/v1")
                .service(
                    web::scope("/publishers")
                        .route("", web::post().to(publisher::create))
                        .route("", web::get().to(publisher::read_all))
                        .route("/{client_id}", web::patch().to(publisher::update))
                        .route("/{client_id}", web::delete().to(publisher::delete))
                )
        );
}

pub fn start(config: &Config) -> Server {
    let db_pool = db::new_pool(&config.database_url)
        .expect("cannot create database pool");

    HttpServer::new(move || {
        let state = AppState {
            db: db_pool.clone(),
        };
        App::new()
            .app_data(Data::new(state))
            .configure(routes)
    })
    .bind(&config.bind_address)
    .unwrap_or_else(|_| panic!("Could not bind server to address {}", &config.bind_address))
    .run()
}
