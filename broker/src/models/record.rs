use diesel::{Insertable, Queryable, Selectable};
use serde::Serialize;
use crate::schema::{events, measurements};
use chrono::naive::serde::ts_seconds::serialize as to_ts;

type State = i32;

const STATE_OFF: State = 0;
const STATE_ON: State = 1;

#[derive(Insertable)]
pub struct Event {
    pub timestamp: chrono::NaiveDateTime,
    pub client_id: String,
    pub device_id: i32,
    pub state: State
}

#[derive(Queryable, Selectable, Serialize)]
#[diesel(table_name = events)]
pub struct ReadEvent {
    #[serde(serialize_with = "to_ts")]
    pub timestamp: chrono::NaiveDateTime,
    pub device_id: i32,
    pub state: State
}

#[derive(Insertable)]
pub struct Measurement {
    pub timestamp: chrono::NaiveDateTime,
    pub client_id: String,
    pub source: String,
    pub value: f32,
    pub quantity: i32
}

#[derive(Queryable, Selectable, Serialize)]
#[diesel(table_name = measurements)]
pub struct ReadMeasurement {
    #[serde(serialize_with = "to_ts")]
    pub timestamp: chrono::NaiveDateTime,
    pub source: String,
    pub value: f32,
    pub quantity: i32
}
