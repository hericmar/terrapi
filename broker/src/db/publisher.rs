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

pub struct UpdatePublisher {
    pub name: Option<String>,
    pub config: Option<String>,
}

pub fn update(conn: &mut DbPooledConnection, client_id: &str, payload: &UpdatePublisher) -> Result<Publisher> {
    Ok(diesel::update(schema::publishers::table
        .filter(schema::publishers::client_id.eq(client_id)))
        .set(payload)
        .returning(Publisher::as_returning())
        .get_result(conn)?)
}

pub fn delete(conn: &mut DbPooledConnection, client_id: &str) -> Result<Publisher> {
    let publisher = read(conn, client_id)?;
    diesel::delete(schema::publishers::table
        .filter(schema::publishers::client_id.eq(client_id)))
        .execute(conn)?;
    Ok(publisher)
}
