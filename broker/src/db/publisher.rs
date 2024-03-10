// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

use diesel::prelude::*;
use crate::app::publisher::CreatePublisher;
use crate::db::DbPooledConnection;
use crate::models::publisher::Publisher;
use crate::{schema, utils};
use crate::prelude::*;

const CLIENT_ID_LENGTH: usize = 8;
const CLIENT_SECRET_LENGTH: usize = 32;

pub fn create(conn: &mut DbPooledConnection, publisher: &CreatePublisher) -> Result<Publisher> {
    // Find a unique client_id
    let mut client_id = utils::random_string(CLIENT_ID_LENGTH);
    while let Ok(_) = read(conn, &client_id) {
        client_id = utils::random_string(CLIENT_ID_LENGTH);
    }

    let payload = Publisher{
        client_id,
        client_secret: utils::random_string(CLIENT_SECRET_LENGTH),
        name: publisher.name.clone(),
        config: "".to_string(),
        created_at: None,
    };

    Ok(diesel::insert_into(schema::publishers::table)
        .values(&payload)
        .returning(Publisher::as_returning())
        .get_result(conn)?)
}

pub fn read(conn: &mut DbPooledConnection, client_id: &str) -> Result<Publisher> {
    Ok(schema::publishers::table
        .filter(schema::publishers::client_id.eq(client_id))
        .first(conn)?)
}

pub fn read_all(conn: &mut DbPooledConnection) -> Result<Vec<Publisher>> {
    Ok(schema::publishers::table
        .load(conn)?)
}

#[derive(AsChangeset)]
#[diesel(table_name = schema::publishers)]
pub struct UpdatePublisher<'a> {
    pub name: Option<&'a String>,
    pub config: Option<&'a String>,
}

pub fn update<'a>(conn: &mut DbPooledConnection, client_id: &str, payload: &UpdatePublisher<'a>) -> Result<Publisher> {
    let updated = diesel::update(schema::publishers::table
        .filter(schema::publishers::client_id.eq(client_id)))
        .set(payload)
        .returning(Publisher::as_returning())
        .get_result(conn)?;

    Ok(updated)
}

pub fn delete(conn: &mut DbPooledConnection, client_id: &str) -> Result<Publisher> {
    let publisher = read(conn, client_id)?;
    diesel::delete(schema::publishers::table
        .filter(schema::publishers::client_id.eq(client_id)))
        .execute(conn)?;
    Ok(publisher)
}
