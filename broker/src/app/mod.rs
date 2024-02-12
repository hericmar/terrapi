pub mod publisher;
pub mod hello;
mod record;
mod auth;

use actix_cors::Cors;
use actix_files::Files;
use actix_identity::IdentityMiddleware;
use actix_session::config::PersistentSession;
use actix_session::SessionMiddleware;
use actix_session::storage::CookieSessionStore;
use actix_web::{
    cookie::{time::Duration, Key},
    App, http, HttpRequest, HttpServer, web
};
use actix_web::cookie::SameSite;
use actix_web::dev::Server;
use actix_web::http::header::{AUTHORIZATION, CONTENT_TYPE};
use actix_web::middleware::Logger;
use actix_web::web::{
    Data,
    service
};
use serde::Deserialize;
use crate::db;
use crate::db::{DbPool, DbPooledConnection};
use crate::error::{Error, ErrorType};
use crate::prelude::*;

/// Deserialized from environment variables using `envy` crate.
#[derive(Clone, Deserialize, Debug)]
pub struct Config {
    pub admin_password: String,
    pub bind_address: String,
    pub database_url: String,
    #[serde(default = "default_static_root")]
    pub static_root: String,
}

fn default_static_root() -> String {
    "./static".to_string()
}

#[derive(Clone)]
pub struct AppState {
    pub db: DbPool,
    pub admin_password: String,
}

fn routes(app: &mut web::ServiceConfig) {
    app
        .service(
            web::scope("/api/v1")
                .service(
                    web::scope("/auth")
                        .route("/login", web::post().to(auth::login))
                        .route("/logout", web::post().to(auth::logout))
                        .route("/user", web::get().to(auth::user))
                )
                .service(
                    web::scope("/hello")
                        .route("", web::put().to(hello::hello)))
                .service(
                    web::scope("/publishers")
                        .route("", web::post().to(publisher::create))
                        .route("", web::get().to(publisher::read_all))
                        .route("/{client_id}", web::patch().to(publisher::update))
                        .route("/{client_id}", web::delete().to(publisher::delete)))
                .service(
                    web::scope("/records")
                        .route("", web::post().to(record::create))
                        .route("", web::get().to(record::read)))
        );
}

pub fn start(config: &Config) -> Server {
    let db_pool = db::new_pool(&config.database_url)
        .expect("cannot create database pool");

    let static_root = config.static_root.clone();

    let state = AppState {
        db: db_pool.clone(),
        admin_password: config.admin_password.clone(),
    };

    let secret_key = Key::generate();

    HttpServer::new(move || {
        /// TODO: Remove `permissive` and configure CORS properly.
        let cors = Cors::permissive()
            // .send_wildcard()
            // .allowed_methods(vec!["GET", "POST"])
            // .allowed_headers(vec![http::header::AUTHORIZATION, http::header::ACCEPT])
            // .allowed_header(http::header::CONTENT_TYPE)
            .max_age(3600);

        App::new()
            .app_data(Data::new(state.clone()))
            .wrap(cors)
            .wrap(Logger::default())
            .wrap(IdentityMiddleware::default())
            .wrap(
                SessionMiddleware::builder(CookieSessionStore::default(), secret_key.clone())
                    // .cookie_domain(Some("localhost:3000".to_owned()))
                    .cookie_name("session".to_owned())
                    .session_lifecycle(PersistentSession::default().session_ttl(Duration::hours(2)))
                    .build()
            )
            .configure(routes)
            .service(Files::new("/", static_root.clone()).index_file("index.html"))
    })
    .bind(&config.bind_address)
    .unwrap_or_else(|_| panic!("Could not bind server to address {}", &config.bind_address))
    .run()
}

pub fn authorize_publisher(
    conn: &mut DbPooledConnection, req: &HttpRequest, client_id: &str
) -> Result<()> {
    let auth_header = req.headers().get(AUTHORIZATION)
        .ok_or(Error::new("Missing token", ErrorType::Unauthorized))?;
    let client_secret = auth_header
        .to_str()
        .map_err(|_| Error::new("Invalid header", ErrorType::Unauthorized))?
        .strip_prefix("Bearer ")
        .ok_or(Error::new("Invalid token", ErrorType::Unauthorized))?;

    let client = db::publisher::read(conn, client_id)?;
    if client.client_secret != client_secret {
        return Err(Error::new("Invalid token", ErrorType::Unauthorized));
    }

    Ok(())
}
