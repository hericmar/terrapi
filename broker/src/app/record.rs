
use std::collections::HashMap;
use actix_web::{HttpResponse, web};
use serde::{Deserialize, Serialize};
use crate::app::AppState;
use crate::db;
use crate::db::record::ReadQuery;
use crate::error::{Error, ErrorType};
use crate::models::record::{Event, Measurement, ReadEvent, ReadMeasurement};
use crate::prelude::*;

use chrono::naive::serde::ts_seconds::deserialize as from_ts;

#[derive(Deserialize)]
pub struct CreateEvent {
    #[serde(deserialize_with = "from_ts")]
    timestamp: chrono::NaiveDateTime,
    source: String,
    state: i32,
}

#[derive(Deserialize)]
pub struct CreateMeasurement {
    #[serde(deserialize_with = "from_ts")]
    timestamp: chrono::NaiveDateTime,
    source: String,
    value: f32,
    quantity: i32,
}

#[derive(Deserialize)]
pub struct CreateRecords {
    pub client_id: String,
    pub events: Vec<CreateEvent>,
    pub measurements: Vec<CreateMeasurement>,
}

pub async fn create(
    ctx: web::Data<AppState>, payload: web::Json<CreateRecords>
) -> Result<HttpResponse> {
    let mut conn = ctx.db.get()?;
    let devices = db::device::read_by_client_id(&mut conn, &payload.client_id)?;

    let devices_map: HashMap<&str, i32> = devices.iter().map(|device| {
        (device.device_id.as_str(), device.id)
    }).collect();

    // validate devices in the events
    for event in &payload.events {
        if !devices_map.contains_key(event.source.as_str()) {
            return Err(Error::new("Device not found", ErrorType::BadRequest));
        }
    }

    let events = payload.events.iter().map(|event| {
        Event {
            timestamp: event.timestamp,
            client_id: payload.client_id.clone(),
            device_id: devices_map[event.source.as_str()],
            state: event.state,
        }
    }).collect();

    let measurements = payload.measurements.iter().map(|measurement| {
        Measurement {
            timestamp: measurement.timestamp,
            client_id: payload.client_id.clone(),
            source: measurement.source.clone(),
            value: measurement.value,
            quantity: measurement.quantity,
        }
    }).collect();
    db::record::create(&mut conn, events, measurements)?;

    Ok(HttpResponse::Ok().finish())
}

#[derive(Serialize)]
pub struct Records {
    pub events: Vec<ReadEvent>,
    pub measurements: Vec<ReadMeasurement>,
}

pub async fn read(
    ctx: web::Data<AppState>, query: web::Query<ReadQuery>
) -> Result<HttpResponse> {
    let (events, measurements) = {
        let mut conn = ctx.db.get()?;
        db::record::read(&mut conn, &query)?
    };
    Ok(HttpResponse::Ok().json(Records { events, measurements }))
}
