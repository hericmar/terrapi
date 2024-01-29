use diesel::PgConnection;
use diesel::r2d2::ConnectionManager;
use r2d2::Pool;

use crate::prelude::*;

pub type DbPool = Pool<ConnectionManager<PgConnection>>;

/// See more: <https://docs.rs/diesel/latest/diesel/r2d2/index.html>.
fn new_pool(database_url: &str) -> Result<DbPool> {
    let manager = ConnectionManager::<PgConnection>::new(database_url);
    let pool = Pool::builder().build(manager)?;

    Ok(pool)
}
