use diesel::{Connection, insert_into, QueryDsl, RunQueryDsl};
use diesel::prelude::*;
use serde::Deserialize;
use crate::{db, schema};
use crate::models::record::{Event, Measurement, ReadEvent, ReadMeasurement};
use crate::prelude::*;

pub fn create(
    conn: &mut db::DbPooledConnection, events: Vec<Event>, measurements: Vec<Measurement>
) -> Result<()> {
    conn.transaction(|conn| {
        insert_into(schema::events::table)
            .values(&events)
            .execute(conn)?;
        insert_into(schema::measurements::table)
            .values(&measurements)
            .execute(conn)?; // TODO: return the number of rows inserted (or an error if none were inserted
        Ok(())
    })
}

use chrono::naive::serde::ts_seconds_option::deserialize as from_tsopt;

#[derive(Deserialize)]
pub struct ReadQuery {
    pub client_id: String,
    #[serde(deserialize_with = "from_tsopt")]
    pub from: Option<chrono::NaiveDateTime>,
    #[serde(deserialize_with = "from_tsopt")]
    pub to: Option<chrono::NaiveDateTime>,
}

pub fn read(
    conn: &mut db::DbPooledConnection, query: &ReadQuery
) -> Result<(Vec<ReadEvent>, Vec<ReadMeasurement>)> {
    let mut events_query = schema::events::table
        .into_boxed()
        .filter(schema::events::client_id.eq(&query.client_id));
    let mut measurements_query = schema::measurements::table
        .into_boxed()
        .filter(schema::measurements::client_id.eq(&query.client_id));

    if let Some(from) = query.from {
        events_query = events_query.filter(schema::events::timestamp.ge(from));
        measurements_query = measurements_query.filter(schema::measurements::timestamp.ge(from));
    }

    if let Some(to) = query.to {
        events_query = events_query.filter(schema::events::timestamp.le(to));
        measurements_query = measurements_query.filter(schema::measurements::timestamp.le(to));
    }

    // We are using custom structs, so we need to use `as_select` to convert them to the correct
    // SQL types.
    let events = events_query
        .select(ReadEvent::as_select())
        .load::<ReadEvent>(conn)?;
    let measurements = measurements_query
        .select(ReadMeasurement::as_select())
        .load::<ReadMeasurement>(conn)?;

    Ok((events, measurements))
}
