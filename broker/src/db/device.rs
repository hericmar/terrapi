use diesel::{ExpressionMethods, Insertable, QueryDsl, RunQueryDsl, SelectableHelper};
use validator::Validate;
use crate::db::DbPooledConnection;
use crate::prelude::*;
use crate::models::device::Device;
use crate::schema;

#[derive(Insertable, Validate)]
#[diesel(table_name = schema::devices)]
pub struct CreateDevice<'a> {
    pub client_id: &'a str,
    #[validate(length(max = 12, message = "larger than 12"))]
    pub device_id: &'a str,
    pub power: Option<f32>,
}

pub fn create(conn: &mut DbPooledConnection, device: &CreateDevice) -> Result<Device> {
    device.validate()?;
    Ok(diesel::insert_into(schema::devices::table)
        .values(device)
        .returning(Device::as_returning())
        .get_result(conn)?)
}

pub fn read_latest(conn: &mut DbPooledConnection, client_id: &str, device_id: &str) -> Result<Device> {
    Ok(schema::devices::table
        .filter(schema::devices::client_id.eq(client_id))
        .filter(schema::devices::device_id.eq(device_id))
        .order(schema::devices::created_at.desc())
        .select(Device::as_select())
        .first(conn)?)
}

pub fn read_by_client_id(conn: &mut DbPooledConnection, client_id: &str) -> Result<Vec<Device>> {
    Ok(schema::devices::table
        .filter(schema::devices::client_id.eq(client_id))
        .select(Device::as_select())
        .load(conn)?)
}
