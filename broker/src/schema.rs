// @generated automatically by Diesel CLI.

diesel::table! {
    publishers (client_id) {
        #[max_length = 8]
        client_id -> Bpchar,
        #[max_length = 32]
        client_secret -> Bpchar,
        #[max_length = 64]
        name -> Varchar,
        config -> Text,
    }
}
