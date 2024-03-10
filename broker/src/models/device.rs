// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

use diesel::{Queryable, Selectable};
use crate::schema::devices;

#[derive(Queryable, Selectable)]
pub struct Device {
    pub(crate) id: i32,
    pub(crate) device_id: String,
    pub power: Option<f32>,
}
