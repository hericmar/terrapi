CREATE TABLE events (
    id SERIAL PRIMARY KEY,
    switch_name VARCHAR,
    state INTEGER,
    datetime TIMESTAMP
);
