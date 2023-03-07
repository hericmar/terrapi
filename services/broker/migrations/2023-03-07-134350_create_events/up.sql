CREATE TABLE events (
    id SERIAL PRIMARY KEY,
    switch_name VARCHAR NOT NULL,
    state INTEGER NOT NULL,
    datetime TIMESTAMP WITH TIME ZONE NOT NULL
);
