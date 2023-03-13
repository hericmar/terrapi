use std::collections::HashMap;
use std::fmt::format;
use std::ops::SubAssign;
use actix_web::{HttpRequest, HttpResponse, web};
use actix_web::http::header::HeaderValue;
use actix_web::http::StatusCode;
use chrono::{DateTime, Duration, FixedOffset, NaiveDateTime, ParseResult, TimeZone, Utc};
use diesel::Queryable;
use rand::distributions::{Alphanumeric, DistString};
use serde::{Deserialize, Serialize};
use crate::error::{Error, ErrorType};
use crate::model::{Client, Event, EventInsert, Measurement, MeasurementInsert};
use crate::{Config, model, repo};
use crate::repo::PostgresPool;
use crate::utils::local_tz_offset;

#[derive(Clone, Debug)]
pub struct Context {
    pub config: Config,
    pub db: PostgresPool,
}

pub fn authorize(password: &String, request: &HttpRequest) -> Result<(), Error> {
    match request.headers().get("Authorization") {
        None => return Err(Error::new("unauthorized", ErrorType::Unauthorized)),
        Some(auth_header) => {
            let split: Vec<&str> = auth_header
                .to_str()
                .unwrap_or("")
                .split("Basic")
                .collect();

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
    // pub timezone_offset: i32,
}

/// GET /api/v1/client
pub async fn list_clients(
    request: HttpRequest, ctx: web::Data<Context>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.admin_password, &request)?;

    Ok(HttpResponse::Ok().json(repo::read_all_clients(&ctx.db)?))
}

/// POST /api/v1/client
pub async fn create_client(
    request: HttpRequest, ctx: web::Data<Context>, payload: web::Json<NewClientRequest>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.admin_password, &request)?;

    let new_client = Client{
        client_id: Alphanumeric.sample_string(&mut rand::thread_rng(), 8),
        password: Alphanumeric.sample_string(&mut rand::thread_rng(), 32),
        name: payload.name.clone(),
        // timezone_offset: payload.timezone_offset,
    };
    Ok(HttpResponse::Ok().json(repo::create_client(&ctx.db, &new_client)?))
}

/// GET /api/v1/client/preview
pub async fn list_client_preview(
    request: HttpRequest, ctx: web::Data<Context>
) -> Result<HttpResponse, Error> {
    let mut clients = repo::read_all_clients(&ctx.db)?;
    for client in &mut clients {
        client.password = "".to_string();
    }

    Ok(HttpResponse::Ok().json(clients))
}

/// POST /api/v1/client/{client_id}
pub async fn delete_client(
    request: HttpRequest, ctx: web::Data<Context>, client_id: web::Path<String>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.admin_password, &request)?;

    Ok(HttpResponse::Ok().json(repo::delete_client(&ctx.db, &client_id.into_inner())?))
}

//----------------------------------------------------------------------------//

/// GET /api/v1/config/{client_id}
pub async fn get_config(
    request: HttpRequest, ctx: web::Data<Context>, client_id: web::Path<String>
) -> Result<HttpResponse, Error> {
    let client = repo::read_client(&ctx.db, &client_id.into_inner())?;

    authorize(&ctx.config.admin_password, &request)?;

    Ok(HttpResponse::Ok().json(repo::read_config(&ctx.db, &client.client_id)?))
}

pub async fn list_configs(
    request: HttpRequest, ctx: web::Data<Context>
) -> Result<HttpResponse, Error> {
    authorize(&ctx.config.admin_password, &request)?;

    Ok(HttpResponse::Ok().json(repo::read_all_configs(&ctx.db)?))
}

/// PUT /api/v1/config
pub async fn put_config(
    request: HttpRequest,
    ctx: web::Data<Context>,
    payload: web::Json<model::Config>
) -> Result<HttpResponse, Error> {
    let config = payload.into_inner();
    let client = repo::read_client(&ctx.db, &config.client_id)?;

    authorize(&client.password, &request)?;

    Ok(HttpResponse::Ok().json(repo::put_config(&ctx.db, &config)?))
}

//----------------------------------------------------------------------------//

#[derive(Serialize, Deserialize)]
pub struct MeasurementRecord {
    pub sensor_name: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub timestamp: u64,
}

#[derive(Serialize, Deserialize)]
pub struct EventRecord {
    pub switch_name: String,
    pub state: bool,
    pub timestamp: u64,
}

#[derive(Deserialize)]
pub struct RecordsRequest {
    pub client_id: String,
    pub measurements: Vec<MeasurementRecord>,
    pub events: Vec<EventRecord>
}

#[derive(Deserialize)]
pub struct QueryParams {
    from: Option<String>,
    to: Option<String>,
}

#[derive(Serialize)]
pub struct MeasurementResponse {
    pub value: f32,
    pub timestamp: u64,
}

#[derive(Serialize)]
pub struct EventResponse {
    pub state: bool,
    pub timestamp: u64,
}

#[derive(Serialize)]
struct RecordsResponse {
    measurements: HashMap<String, Vec<MeasurementResponse>>,
    events: HashMap<String, Vec<EventResponse>>
}

fn default_datetime(hours_to_subtract: i64) -> DateTime<Utc> {
    let mut value = Utc::now();
    value.sub_assign(Duration::hours(hours_to_subtract));

    value
}

/// GET /api/v1/records/{client_id}?from={f}&to={t}
///
/// param `from` and `to` in format 2017-08-07T12:09:23.555+01:00, keep in mind, that these values
/// must be URL encoded, e.g.
/// ```
/// /api/v1/record/yFBgI2YP?from=2023-03-06T23%3A59%3A01%2B01%3A00&to=2023-03-16T23%3A39%3A25%2B01%3A00
/// ```
pub async fn get_records(
    ctx: web::Data<Context>, client_id: web::Path<String>, query: web::Query<QueryParams>
) -> Result<HttpResponse, Error> {
    let client_id = client_id.into_inner();

    let _ = repo::read_client(&ctx.db, &client_id)?;

    let from = match &query.from {
        None => {
            default_datetime(24)
        },
        Some(value) => {
            match Utc.datetime_from_str(value, "%Y-%m-%dT%H:%M:%S%.3fZ") {
                Ok(value) => value.with_timezone(&Utc),
                Err(_) => default_datetime(24)
            }
        },
    };

    println!("{}", &query.to.clone().unwrap());

    let to = match &query.to {
        None => {
            default_datetime(0)
        },
        Some(value) => {
            match Utc.datetime_from_str(value, "%Y-%m-%dT%H:%M:%S%.3fZ") {
                Ok(value) => value.with_timezone(&Utc),
                Err(_) => default_datetime(0)
            }
        },
    };

    let measurements = repo::read_measurement_records(&ctx.db, &client_id, from, to, 200)?
        .into_iter()
        .fold(HashMap::new(), |mut map, item| {
            let key = format!("{} {}", item.sensor_name, item.physical_quantity);
            map.entry(key).or_insert(vec![]).push(MeasurementResponse{
                value: item.value,
                timestamp: item.datetime.timestamp() as u64,
            });
            map
        });

    let events = repo::read_event_records(&ctx.db, &client_id, from, to, 100)?
        .into_iter()
        .fold(HashMap::new(), |mut map, item| {
            map.entry(item.switch_name.clone()).or_insert(vec![]).push(EventResponse{
                state: if item.state == 1 {true} else {false},
                timestamp: item.datetime.timestamp() as u64,
            });
            map
        });

    Ok(HttpResponse::Ok().json(RecordsResponse{
        measurements, events
    }))
}

/// POST /api/v1/records
pub async fn post_records(
    request: HttpRequest, ctx: web::Data<Context>, payload: web::Json<RecordsRequest>
) -> Result<HttpResponse, Error> {
    let client = repo::read_client(&ctx.db, &payload.client_id)?;

    authorize(&client.password, &request)?;

    repo::create_measurement_records(
        &ctx.db, 
        &payload.measurements.iter().map(|item| {
            let naive = NaiveDateTime::from_timestamp_opt(item.timestamp as i64, 0).unwrap();
            let mut datetime: DateTime<Utc> = DateTime::from_utc(naive, Utc);
            // datetime.sub_assign(Duration::hours(local_tz_offset() as i64));
            MeasurementInsert{
                client_id: client.client_id.clone(),
                sensor_name: item.sensor_name.clone(),
                value: item.value,
                physical_quantity: item.physical_quantity,
                datetime,
            }
        }).collect()
    )?;
    repo::create_event_records(
        &ctx.db, 
        &payload.events.iter().map(|item| {
            let naive = NaiveDateTime::from_timestamp_opt(item.timestamp as i64, 0).unwrap();
            let mut datetime: DateTime<Utc> = DateTime::from_utc(naive, Utc);
            // datetime.sub_assign(Duration::hours(local_tz_offset() as i64));
            EventInsert{
                client_id: client.client_id.clone(),
                switch_name: item.switch_name.clone(),
                state: if item.state {1} else {0},
                datetime,
            }
        }).collect()
    )?;

    Ok(HttpResponse::new(StatusCode::CREATED))
}

//----------------------------------------------------------------------------//

#[derive(Deserialize)]
pub struct LoginRequest {
    pub password: String
}

/// POST /api/v1/login
pub async fn login(
    ctx: web::Data<Context>, payload: web::Json<LoginRequest>
) -> Result<HttpResponse, Error> {
    if ctx.config.admin_password != payload.password {
        return Ok(HttpResponse::new(StatusCode::UNAUTHORIZED))
    }
    Ok(HttpResponse::new(StatusCode::OK))
}
