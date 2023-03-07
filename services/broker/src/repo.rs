use diesel::pg::PgConnection;
use diesel::r2d2::{ConnectionManager, Pool};
use crate::error::Error;

pub type PostgresPool = Pool<ConnectionManager<PgConnection>>;

pub fn create_conn_pool(url: &String) -> PostgresPool {
    let manager = ConnectionManager::<PgConnection>::new(url);
    Pool::builder()
        .build(manager)
        .expect("could not build connection pool")
}
