CREATE TABLE measurements (
    id SERIAL PRIMARY KEY,
    client_id CHAR(8) NOT NULL,
    sensor_name VARCHAR NOT NULL,
    value REAL NOT NULL,
    physical_quantity INTEGER NOT NULL,
    datetime TIMESTAMP WITH TIME ZONE NOT NULL
)
