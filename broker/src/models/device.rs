use diesel::{Queryable, Selectable};
use crate::schema::devices;

#[derive(Queryable, Selectable)]
pub struct Device {
    id: i32,
    device_id: String,
    pub power: Option<f32>,
}
