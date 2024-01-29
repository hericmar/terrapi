# TerraPi Broker

## Configuration
| Environment Variable | Description |
| -------------------- | ----------- |
| `DATABASE_URL`       | Database URL |

## Development
```shell
cp .env.example .env
```

### Diesel
Install diesel cli
```shell
cargo install diesel_cli --no-default-features --features postgres,sqlite

diesel setup
```

```shell
DATABASE_URL=postgres://terrapi:password@localhost/terrapi diesel migration generate create_measurements
DATABASE_URL=postgres://terrapi:password@localhost/terrapi diesel migration generate create_events

# Will create schema for tables.
DATABASE_URL=postgres://terrapi:password@localhost/terrapi diesel migration run
```
