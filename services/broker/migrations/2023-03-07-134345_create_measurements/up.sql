CREATE TABLE measurements (
    id SERIAL PRIMARY KEY,
    sensor_name VARCHAR NOT NULL,
    value REAL NOT NULL,
    physical_quantity INTEGER NOT NULL,
    datetime TIMESTAMP WITH TIME ZONE NOT NULL
)
