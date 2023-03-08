use chrono::{DateTime, FixedOffset, Utc};
use diesel::pg::PgConnection;
use diesel::{debug_query, ExpressionMethods, insert_into, QueryDsl, RunQueryDsl};
use diesel::r2d2::{ConnectionManager, Pool};
use crate::error::{Error, ErrorType};
use crate::model::{Client, Config, Event, EventInsert, Measurement, MeasurementInsert};
use crate::schema;

pub type PostgresPool = Pool<ConnectionManager<PgConnection>>;

impl From<diesel::result::Error> for Error {
    fn from(value: diesel::result::Error) -> Self {
        match value {
            diesel::result::Error::NotFound => Error::new(&value.to_string(), ErrorType::NotFound),
            _ => Error::new(&value.to_string(), ErrorType::DatabaseError)
        }
    }
}

pub fn create_conn_pool(url: &String) -> PostgresPool {
    let manager = ConnectionManager::<PgConnection>::new(url);
    Pool::builder()
        .build(manager)
        .expect("could not build connection pool")
}

//

pub fn read_client(db: &PostgresPool, client_id: &String) -> Result<Client, Error> {
    let mut conn = db.get()?;
    let result = schema::clients::dsl::clients
        .filter(schema::clients::client_id.eq(client_id))
        .first(&mut conn)?;

    Ok(result)
}

pub fn read_all_clients(db: &PostgresPool) -> Result<Vec<Client>, Error> {
    let mut conn = db.get()?;
    let result: Vec<Client> = schema::clients::dsl::clients
        .select(schema::clients::all_columns)
        .load::<Client>(&mut conn)?;

    Ok(result)
}

pub fn create_client(db: &PostgresPool, client: &Client) -> Result<Client, Error> {
    let mut conn = db.get()?;
    Ok(
        insert_into(schema::clients::dsl::clients)
            .values(client)
            .get_result(&mut conn)?
    )
}

pub fn delete_client(db: &PostgresPool, client_id: &String) -> Result<usize, Error> {
    let mut conn = db.get()?;
    Ok(
        diesel::delete(schema::clients::dsl::clients.find(client_id))
            .execute(&mut conn)?
    )
}

//

pub fn read_config(db: &PostgresPool, client_id: &String) -> Result<Config, Error> {
    let mut conn = db.get()?;
    Ok(
        schema::configs::dsl::configs
            .filter(schema::configs::client_id.eq(client_id))
            .first(&mut conn)?
    )
}

pub fn read_all_configs(db: &PostgresPool) -> Result<Vec<Config>, Error> {
    let mut conn = db.get()?;

    let result: Vec<Config> = schema::configs::dsl::configs
        .select(schema::configs::all_columns)
        .load::<Config>(&mut conn)?;

    Ok(result)
}

pub fn insert_config(db: &PostgresPool, config: &Config) -> Result<usize, Error> {
    let mut conn = db.get()?;
    Ok(
        insert_into(schema::configs::dsl::configs)
            .values(config)
            .execute(&mut conn)?
    )
}

//

pub fn read_measurement_records(
    db: &PostgresPool,
    client_id: &String,
    from: DateTime<Utc>,
    to: DateTime<Utc>,
    limit_value: i64
) -> Result<Vec<Measurement>, Error> {
    let mut conn = db.get()?;

    let result: Vec<Measurement> = schema::measurements::dsl::measurements
        .limit(limit_value)
        // .offset(offset_value)
        .filter(schema::measurements::dsl::client_id.eq(client_id))
        .filter(schema::measurements::dsl::datetime.gt(from))
        .filter(schema::measurements::dsl::datetime.lt(to))
        .load::<Measurement>(&mut conn)?;

    Ok(result)
}

pub fn create_measurement_records(
    db: &PostgresPool, payload: &Vec<MeasurementInsert>
) -> Result<usize, Error> {
    let mut conn = db.get()?;

    Ok(insert_into(schema::measurements::dsl::measurements)
        .values(payload)
        .execute(&mut conn)?)
}

pub fn read_event_records(
    db: &PostgresPool,
    client_id: &String,
    from: DateTime<Utc>,
    to: DateTime<Utc>,
    limit_value: i64
) -> Result<Vec<Event>, Error> {
    let mut conn = db.get()?;

    let result: Vec<Event> = schema::events::dsl::events
        .limit(limit_value)
        // .offset(offset_value)
        .filter(schema::events::dsl::client_id.eq(client_id))
        .filter(schema::events::dsl::datetime.gt(from))
        .filter(schema::events::dsl::datetime.lt(to))
        .load::<Event>(&mut conn)?;

    Ok(result)
}

pub fn create_event_records(
    db: &PostgresPool, payload: &Vec<EventInsert>
) -> Result<usize, Error> {
    let mut conn = db.get()?;

    Ok(insert_into(schema::events::dsl::events)
        .values(payload)
        .execute(&mut conn)?)
}
