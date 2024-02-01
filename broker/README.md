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
diesel migration generate create_posts

# Will create schema for tables.
diesel migration run
```
