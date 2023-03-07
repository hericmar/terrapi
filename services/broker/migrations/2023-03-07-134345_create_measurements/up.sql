CREATE TABLE measurements (
    id SERIAL PRIMARY KEY,
    sensor_name VARCHAR,
    value REAL,
    physical_quantity INTEGER,
    datetime TIMESTAMP
)
