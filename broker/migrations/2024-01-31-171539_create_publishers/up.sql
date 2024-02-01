-- Your SQL goes here
CREATE TABLE publishers (
    client_id CHAR(8) PRIMARY KEY,
    client_secret CHAR(32) NOT NULL,
    name VARCHAR(64) NOT NULL,
    config TEXT NOT NULL DEFAULT ''
);
