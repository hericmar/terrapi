// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>


use std::collections::HashMap;
use actix_web::{HttpRequest, HttpResponse, web};
use serde::{Deserialize, Serialize};
use crate::app::{AppState, authorize_publisher};
use crate::db;
use crate::db::record::ReadQuery;
use crate::error::{Error, ErrorType};
use crate::models::record::{Event, Measurement, ReadEvent, ReadMeasurement};
use crate::prelude::*;

use chrono::naive::serde::ts_seconds::deserialize as from_ts;
use crate::models::device::Device;

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
    ctx: web::Data<AppState>, req: HttpRequest, payload: web::Json<CreateRecords>
) -> Result<HttpResponse> {
    let mut conn = ctx.db.get()?;

    authorize_publisher(&mut conn, &req, &payload.client_id)?;

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

use chrono::naive::serde::ts_seconds::serialize as to_ts;

#[derive(Serialize)]
pub struct EventResponse {
    #[serde(serialize_with = "to_ts")]
    pub timestamp: chrono::NaiveDateTime,
    pub source: String,
    pub power: i32,
    pub state: i32,
}

#[derive(Serialize)]
pub struct Records {
    pub events: Vec<EventResponse>,
    pub measurements: Vec<ReadMeasurement>,
}

pub async fn read(
    ctx: web::Data<AppState>, query: web::Query<ReadQuery>
) -> Result<HttpResponse> {
    let mut conn = ctx.db.get()?;

    let (events, measurements) = db::record::read(&mut conn, &query)?;

    let devices_map: HashMap<i32, Device> = db::device::read_by_client_id(&mut conn, &query.client_id)?
        .into_iter().map(|device| {
            (device.id, device)
        }).collect();

    let processed_events = events.into_iter().map(|event| {
        let device = &devices_map[&event.device_id];
        EventResponse {
            timestamp: event.timestamp,
            source: device.device_id.clone(),
            power: device.power.unwrap_or(0.0) as i32,
            state: event.state,
        }
    }).collect::<Vec<EventResponse>>();

    Ok(HttpResponse::Ok().json(Records { events: processed_events, measurements }))
}
