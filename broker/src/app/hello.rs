// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

use std::collections::HashMap;
use actix_web::{HttpRequest, HttpResponse, web};
use actix_web::http::StatusCode;
use diesel::Connection;
use serde::Deserialize;
use validator::Validate;
use crate::app::{AppState, authorize_publisher};
use crate::db;
use crate::error::{Error, ErrorType};
use crate::prelude::*;

const EPSILON: f32 = 0.001;

#[derive(Deserialize)]
pub struct Hello {
    client_id: String,
    config: String
}

#[derive(Deserialize)]
struct DeviceConfig {
    power: Option<f32>
}

#[derive(Deserialize)]
struct Config {
    #[serde(rename(deserialize = "device"))]
    devices: Option<HashMap<String, DeviceConfig>>,
}

impl From<toml::de::Error> for Error {
    fn from(error: toml::de::Error) -> Self {
        Error::new(&error.to_string(), ErrorType::BadRequest)
    }
}

/// PUT /api/v1/hello
pub async fn hello(
    ctx: web::Data<AppState>, req: HttpRequest, payload: web::Json<Hello>
) -> Result<HttpResponse> {
    let conn = &mut ctx.db.get()?;

    authorize_publisher(conn, &req, &payload.client_id)?;

    conn.transaction(|conn| {
        process_hello(conn, &payload)
    })?;

    Ok(HttpResponse::Ok().status(StatusCode::NO_CONTENT).finish())
}

fn process_hello(
    conn: &mut db::DbPooledConnection,
    payload: &Hello
) -> Result<()> {
    let config: Config = toml::from_str(&payload.config)?;
    let devices = config.devices.unwrap_or_default();

    for (device_id, device_config) in devices.iter() {
        let read_result = db::device::read_latest(conn, &payload.client_id, device_id);
        let mut create_new_device = false;
        match read_result {
            Ok(device) => {
                let delta = device_config.power.unwrap_or(0.0) - device.power.unwrap_or(0.0);
                if delta.abs() > EPSILON {
                    // Device power has changed.
                    create_new_device = true;
                }
            }
            Err(err) => {
                match err.err_type {
                    ErrorType::NotFound => {
                        // Create the device if it doesn't exist.
                        create_new_device = true;
                    },
                    _ => return Err(err)
                }
            }
        }

        if create_new_device {
            let data = &db::device::CreateDevice{
                client_id: &payload.client_id,
                device_id,
                power: device_config.power,
            };
            db::device::create(conn, data)?;
        }
    }

    let data = &db::publisher::UpdatePublisher{
        name: None,
        config: Some(&payload.config),
    };
    db::publisher::update(conn, &payload.client_id, &data)?;

    Ok(())
}
