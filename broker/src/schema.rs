// SPDX-License-Identifier: MPL-2.0
// Copyright (c) 2020-2024 Martin Herich <martin.herich@phire.cz>

// @generated automatically by Diesel CLI.

diesel::table! {
    devices (id) {
        id -> Int4,
        #[max_length = 8]
        client_id -> Bpchar,
        #[max_length = 12]
        device_id -> Varchar,
        created_at -> Timestamp,
        power -> Nullable<Float4>,
    }
}

diesel::table! {
    events (id) {
        id -> Int4,
        timestamp -> Timestamp,
        #[max_length = 8]
        client_id -> Bpchar,
        device_id -> Int4,
        state -> Int4,
    }
}

diesel::table! {
    measurements (id) {
        id -> Int4,
        timestamp -> Timestamp,
        #[max_length = 8]
        client_id -> Bpchar,
        #[max_length = 12]
        source -> Varchar,
        value -> Float4,
        quantity -> Int4,
    }
}

diesel::table! {
    publishers (client_id) {
        #[max_length = 8]
        client_id -> Bpchar,
        #[max_length = 32]
        client_secret -> Bpchar,
        #[max_length = 64]
        name -> Varchar,
        config -> Text,
        created_at -> Nullable<Timestamp>,
    }
}

diesel::table! {
    records (event_id, measurement_id) {
        event_id -> Int4,
        measurement_id -> Int4,
    }
}

diesel::joinable!(events -> devices (device_id));
diesel::joinable!(events -> publishers (client_id));
diesel::joinable!(measurements -> publishers (client_id));
diesel::joinable!(records -> events (event_id));
diesel::joinable!(records -> measurements (measurement_id));

diesel::allow_tables_to_appear_in_same_query!(
    devices,
    events,
    measurements,
    publishers,
    records,
);
