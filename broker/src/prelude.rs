// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

use crate::error;

pub type Result<T, E = error::Error> = std::result::Result<T, E>;
