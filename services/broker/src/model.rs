use chrono::{DateTime, FixedOffset, Utc};
use diesel::{AsExpression, Insertable, Queryable};
use diesel::sql_types::Timestamptz;
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

#[derive(Queryable, Insertable, Serialize)]
pub struct Measurement {
    pub id: i32,
    pub sensor_name: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub datetime: DateTime<Utc>,
}

#[derive(Queryable, Insertable, Serialize)]
pub struct Event {
    pub id: i32,
    pub switch_name: String,
    pub state: i32,
    pub datetime: DateTime<Utc>,
}
