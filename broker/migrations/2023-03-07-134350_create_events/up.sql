CREATE TABLE events (
    id SERIAL PRIMARY KEY,
    client_id CHAR(8) NOT NULL,
    switch_name VARCHAR NOT NULL,
    state INTEGER NOT NULL,
    datetime TIMESTAMP WITH TIME ZONE NOT NULL
);
