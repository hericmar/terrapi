-- Your SQL goes here
ALTER TABLE publishers ADD COLUMN created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP;

CREATE TABLE devices (
    id SERIAL PRIMARY KEY,
    client_id CHAR(8) NOT NULL,
    device_id CHAR(12) NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    power REAL,
    UNIQUE (client_id, device_id, created_at)
);

CREATE TABLE events (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    client_id CHAR(8) NOT NULL,
    device_id INT NOT NULL,
    state INT NOT NULL,
    UNIQUE (timestamp, client_id, device_id),
    FOREIGN KEY (client_id) REFERENCES publishers(client_id) ON DELETE SET DEFAULT,
    FOREIGN KEY (device_id) REFERENCES devices(id) ON DELETE RESTRICT
);

CREATE TABLE measurements (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    client_id CHAR(8) NOT NULL,
    sensor_id CHAR(12) NOT NULL,
    value REAL NOT NULL,
    UNIQUE (timestamp, client_id, sensor_id),
    FOREIGN KEY (client_id) REFERENCES publishers(client_id) ON DELETE SET DEFAULT
);

CREATE TABLE records (
    event_id INT NOT NULL,
    measurement_id INT NOT NULL,
    PRIMARY KEY (event_id, measurement_id),
    FOREIGN KEY (event_id) REFERENCES events(id) ON DELETE RESTRICT,
    FOREIGN KEY (measurement_id) REFERENCES measurements(id) ON DELETE RESTRICT
)
