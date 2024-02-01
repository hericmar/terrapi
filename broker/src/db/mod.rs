pub mod publisher;
mod hello;

use diesel::PgConnection;
use diesel::result::DatabaseErrorKind;
use diesel::{prelude::*, r2d2};
use crate::error::{Error, ErrorType};

use crate::prelude::*;

pub type DbPool = r2d2::Pool<r2d2::ConnectionManager<PgConnection>>;
pub type DbPooledConnection = r2d2::PooledConnection<r2d2::ConnectionManager<PgConnection>>;

type DieselError = diesel::result::Error;
impl From<DieselError> for Error {
    fn from(error: DieselError) -> Self {
        match error {
            DieselError::DatabaseError(kind, info) => {
                if let DatabaseErrorKind::UniqueViolation = kind {
                    let message = info.details().unwrap_or_else(|| info.message()).to_string();
                    return Error::new(&message, ErrorType::BadRequest);
                }
                Error::new("Internal server error", ErrorType::InternalError)
            }
            DieselError::NotFound => {
                Error::new("Not found", ErrorType::NotFound)
            }
            _ => Error::new("Internal server error", ErrorType::InternalError),
        }
    }
}

impl From<r2d2::Error> for Error {
    fn from(error: r2d2::Error) -> Self {
        Error::new(&error.to_string(), ErrorType::DatabaseError)
    }
}

/// See more: <https://docs.rs/diesel/latest/diesel/r2d2/index.html>.
pub fn new_pool(database_url: &str) -> Result<DbPool> {
    let manager = r2d2::ConnectionManager::<PgConnection>::new(database_url);
    let pool = r2d2::Pool::builder().build(manager)?;

    Ok(pool)
}

pub fn get_connection(pool: &DbPool) -> Result<DbPooledConnection> {
    pool.get().map_err(|e| e.into())
}
