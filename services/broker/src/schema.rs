// @generated automatically by Diesel CLI.

diesel::table! {
    clients (client_id) {
        client_id -> Bpchar,
        password -> Varchar,
        name -> Varchar,
        timezone_offset -> Int4,
    }
}

diesel::table! {
    configs (client_id) {
        client_id -> Bpchar,
        config -> Text,
    }
}

diesel::table! {
    events (id) {
        id -> Int4,
        switch_name -> Varchar,
        state -> Int4,
        datetime -> Timestamptz,
    }
}

diesel::table! {
    measurements (id) {
        id -> Int4,
        sensor_name -> Varchar,
        value -> Float4,
        physical_quantity -> Int4,
        datetime -> Timestamptz,
    }
}

diesel::allow_tables_to_appear_in_same_query!(
    clients,
    configs,
    events,
    measurements,
);
