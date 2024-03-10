// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

use diesel::prelude::*;
use serde::{Deserialize, Serialize};
use crate::schema::publishers;

#[derive(Queryable, Insertable, Selectable, Serialize, Deserialize)]
pub struct Publisher {
    pub client_id: String,
    pub client_secret: String,
    pub name: String,
    pub config: String,
    pub created_at: Option<chrono::NaiveDateTime>,
}
