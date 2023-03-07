use diesel::pg::PgConnection;
use diesel::{ExpressionMethods, insert_into, QueryDsl, RunQueryDsl};
use diesel::r2d2::{ConnectionManager, Pool};
use crate::error::{Error, ErrorType};
use crate::model::Client;
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

pub fn delete_client(db: &PostgresPool, client_id: &String) -> Result<i32, Error> {
    let mut conn = db.get()?;
    let num_deleted = diesel::delete(
        schema::clients::dsl::clients.find(client_id)
    ).execute(&mut conn)?;

    Ok(num_deleted as i32)
}
