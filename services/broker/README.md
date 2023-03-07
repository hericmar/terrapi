## Development
### Diesel
```shell
DATABASE_URL=postgres://terrapi:password@localhost/terrapi diesel migration generate create_measurements
DATABASE_URL=postgres://terrapi:password@localhost/terrapi diesel migration generate create_events

# Will create schema for tables.
DATABASE_URL=postgres://terrapi:password@localhost/terrapi diesel migration run
```
