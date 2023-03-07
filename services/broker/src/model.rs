use chrono::{DateTime, FixedOffset};
use diesel::{Insertable, Queryable};
use serde::{Deserialize, Serialize};
use crate::schema::*;

#[derive(Queryable, Insertable, Serialize)]
pub struct Client {
    pub client_id: String,
    pub password: String,
    pub name: String,
    pub timezone_offset: i32,
}

#[derive(Queryable, Insertable, Serialize, Deserialize)]
pub struct Config {
    pub client_id: String,
    pub config: String,
}

#[derive(Queryable)]
struct MeasurementRecord {
    pub sensor_name: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub timestamp: DateTime<FixedOffset>,
}

#[derive(Queryable)]
struct EventRecord {
    pub switch_name: String,
    pub state: bool,
    pub timestamp: DateTime<FixedOffset>,
}
