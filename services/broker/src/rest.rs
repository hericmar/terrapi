use std::collections::HashMap;
use actix_web::{HttpRequest, HttpResponse, web};
use actix_web::http::header::HeaderValue;
use actix_web::http::StatusCode;
use chrono::{DateTime, FixedOffset, Utc};
use rand::distributions::{Alphanumeric, DistString};
use serde::{Deserialize, Serialize};
use crate::error::{Error, ErrorType};
use crate::model::Client;
use crate::{Config, model, repo};
use crate::repo::PostgresPool;

#[derive(Clone, Debug)]
pub struct Context {
    pub config: Config,
    pub db: PostgresPool
}

pub fn authorize(password: &String, request: &HttpRequest) -> Result<(), Error> {
    match request.headers().get("Authorization") {
        None => return Err(Error::new("unauthorized", ErrorType::Unauthorized)),
        Some(auth_header) => {
            let split: Vec<&str> = auth_header.to_str().unwrap_or("").split("Basic").collect();
            if split.len() != 2 {
                return Err(Error::new("missing Basic value", ErrorType::Unauthorized));
            }

            let given_password = split[1].trim();

            if password != given_password {
                return Err(Error::new("bad password", ErrorType::Unauthorized))
            }
        }
    };

    Ok(())
}

//----------------------------------------------------------------------------//

/// GET /api/v1/client/{client_id}
pub async fn get_client(
    ctx: web::Data<Context>, client_id: web::Path<String>
) -> Result<HttpResponse, Error> {
    let result = repo::read_client(&ctx.db, &client_id.into_inner())?;

    Ok(HttpResponse::Ok().json(result))
}

#[derive(Debug, Serialize, Deserialize)]
pub struct NewClientRequest {
    pub name: String,
    pub timezone_offset: i32,
}

/// GET /api/v1/client
pub async fn list_clients(
    request: HttpRequest, ctx: web::Data<Context>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.password, &request)?;

    Ok(HttpResponse::Ok().json(repo::read_all_clients(&ctx.db)?))
}

/// POST /api/v1/client
pub async fn create_client(
    request: HttpRequest, ctx: web::Data<Context>, payload: web::Json<NewClientRequest>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.password, &request)?;

    let new_client = Client{
        client_id: Alphanumeric.sample_string(&mut rand::thread_rng(), 8),
        password: Alphanumeric.sample_string(&mut rand::thread_rng(), 32),
        name: payload.name.clone(),
        timezone_offset: payload.timezone_offset,
    };
    Ok(HttpResponse::Ok().json(repo::create_client(&ctx.db, &new_client)?))
}

/// POST /api/v1/client/{client_id}
pub async fn delete_client(
    request: HttpRequest, ctx: web::Data<Context>, client_id: web::Path<String>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.password, &request)?;

    Ok(HttpResponse::Ok().json(repo::delete_client(&ctx.db, &client_id.into_inner())?))
}

//----------------------------------------------------------------------------//

/// GET /api/v1/config/{client_id}
pub async fn get_config(
    request: HttpRequest, ctx: web::Data<Context>, client_id: web::Path<String>
) -> Result<HttpResponse, Error> {
    let client = repo::read_client(&ctx.db, &client_id.into_inner())?;

    authorize(&ctx.config.password, &request)?;

    Ok(HttpResponse::Ok().json(repo::read_config(&ctx.db, &client.client_id)?))
}

/// PUT /api/v1/config
pub async fn put_config(
    request: HttpRequest, ctx: web::Data<Context>, payload: web::Json<model::Config>
) -> Result<HttpResponse, Error> {
    let config = payload.into_inner();
    let client = repo::read_client(&ctx.db, &config.client_id)?;

    authorize(&client.password, &request)?;

    Ok(HttpResponse::Ok().json(repo::insert_config(&ctx.db, &config)?))
}

//----------------------------------------------------------------------------//

pub struct MeasurementRecord {
    pub sensor_name: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub timestamp: u64,
}

pub struct EventRecord {
    pub switch_name: String,
    pub state: bool,
    pub timestamp: u64,
}

pub struct RecordsRequest {
    pub client_id: String,
    pub measurements: Vec<MeasurementRecord>,
    pub events: Vec<EventRecord>
}

#[derive(Deserialize)]
struct QueryParams {
    from: Option<String>,
    to: Option<String>,
}

pub struct MeasurementResponse {
    pub value: f32,
    pub timestamp: u64,
}

pub struct EventResponse {
    pub state: bool,
    pub timestamp: u64,
}

#[derive(Serialize)]
struct RecordsResponse {
    measurements: HashMap<String, Vec<MeasurementRecord>>,
    events: HashMap<String, Vec<EventRecord>>
}

/// GET /api/v1/records/{client_id}?from={f}&to={t}
///
/// param `from` and `to` in format 2017-08-07T12:09:23.555+01:00
pub async fn get_records(
    ctx: web::Data<Context>, client_id: web::Path<String>, query: web::Query<QueryParams>
) -> Result<HttpResponse, Error> {
    let from = match &query.from {
        None => {
            let fixed_offset = FixedOffset::west_opt(0);
            fixed_offset.now()
        },
        Some(value) => DateTime::parse_from_str(value, "%Y-%m-%dT%H:%M:%S%z")?,
    };

    let to = match &query.to {
        None => {
            let fixed_offset = FixedOffset::west_opt(0);
            fixed_offset.now()
        },
        Some(value) => DateTime::parse_from_str(value, "%Y-%m-%dT%H:%M:%S%z")?,
    };

    /*
        let age_map: HashMap<i32, Vec<&Person>> = people
        .iter()
        .group_by(|person| person.age)
        .into_iter()
        .map(|(age, group)| (age, group.collect()))
        .collect();
     */

    let measurements = repo::read_measurement_records(&ctx.db, &client_id.into_inner()?, from, to, 200)?
        .iter()
        .map(|record| MeasurementResponse{
            value: record.value, timestamp: record.timestamp
        });

    /*
    let measurements = repo::read_measurement_records(&ctx.db, &client_id.into_inner()?, from, to, 200)?
        .iter()
        .map(|record| MeasurementResponse{
            value: record.value, timestamp: record.timestamp
        });
    let events = repo::read_event_records(&ctx.db, &client_id.into_inner()?, from, to, 200)?
        .iter()
        .map(|event| EventResponse{
            state: event.state,
            timestamp: event.timestamp,
        });
     */

    Ok(HttpResponse::new(StatusCode::NO_CONTENT))
}

/// POST /api/v1/records
pub async fn post_records(
    ctx: web::Data<Context>, payload: web::Json<RecordsRequest>
) -> Result<HttpResponse, Error> {


    Ok(HttpResponse::new(StatusCode::NO_CONTENT))
}
