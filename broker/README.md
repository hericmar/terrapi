# TerraPi Broker

## Installation
Run `make && make install` to install the broker.

Next you'll need to create postgres database and run migrations.
We use diesel cli for this purpose.

```shell
diesel migration run
```

## Configuration
Run broker with environment variables or set path to `.env` file by `--env-file` argument.
See [.env.example](.env.example) for more configuration options.

| Environment Variable | Description |
| -------------------- | ----------- |
| `DATABASE_URL`       | Database URL |

Directory etc/broker contains other configuration files for the broker,
such as nginx configuration, and systemd service.

## Development
```shell
cp .env.example .env
```

### Diesel
Install diesel cli
```shell
cargo install diesel_cli --no-default-features --features postgres,sqlite

diesel setup
diesel migration generate create_posts

# Will create schema for tables.
diesel migration run
```
