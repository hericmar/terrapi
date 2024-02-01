mod error;
mod schema;

mod app;
mod db;
mod prelude;
mod models;

mod utils;

use std::io::Write;
use clap::Parser;
use dotenv;
use serde::Deserialize;
use crate::app::Config;

#[derive(Parser, Debug)]
#[command(author, version, about, long_about = None)]
struct Args {
    #[arg(short, long)]
    env_file: Option<String>
}

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    std::env::set_var("RUST_LOG", "debug");
    env_logger::init();

    let args = Args::parse();

    if let Some(env_file) = args.env_file {
        dotenv::from_filename(&env_file)
            .expect(&format!("cannot read env file {}", &env_file));
    }

    let config = match envy::prefixed("TERRA_").from_env::<Config>() {
        Ok(config) => { config }
        Err(err) => { panic!("cannot parse config: {}", err); }
    };

    app::start(&config).await
}
