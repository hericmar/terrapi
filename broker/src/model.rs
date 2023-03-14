use chrono::{DateTime, FixedOffset, Utc};
use diesel::{AsExpression, AsChangeset, Insertable, Queryable};
use diesel::sql_types::Timestamptz;
use serde::{Deserialize, Serialize};
use crate::schema::*;

#[derive(Queryable, AsChangeset, Insertable, Serialize)]
pub struct Client {
    pub client_id: String,
    pub password: String,
    pub name: String,
    // pub timezone_offset: i32,
}

#[derive(Queryable, Insertable, AsChangeset, Serialize, Deserialize)]
pub struct Config {
    pub client_id: String,
    pub config: String,
}

#[derive(Queryable, Serialize)]
pub struct Measurement {
    pub id: i32,
    pub client_id: String,
    pub sensor_name: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub datetime: DateTime<Utc>,
}

#[derive(Queryable, Insertable, Serialize)]
#[diesel(table_name = measurements)]
pub struct MeasurementInsert {
    pub sensor_name: String,
    pub client_id: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub datetime: DateTime<Utc>,
}

#[derive(Queryable, Serialize)]
pub struct Event {
    pub id: i32,
    pub client_id: String,
    pub switch_name: String,
    pub state: i32,
    pub datetime: DateTime<Utc>,
}

#[derive(Queryable, Insertable, Serialize)]
#[diesel(table_name = events)]
pub struct EventInsert {
    pub client_id: String,
    pub switch_name: String,
    pub state: i32,
    pub datetime: DateTime<Utc>,
}
