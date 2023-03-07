CREATE TABLE clients (
    client_id CHAR(8) PRIMARY KEY,
    password VARCHAR NOT NULL,
    name VARCHAR NOT NULL,
    timezone_offset INTEGER NOT NULL
);
