use std::fmt::{Display, Formatter};
use std::sync::{MutexGuard, PoisonError};
use actix_web::http::StatusCode;
use actix_web::{HttpResponse, ResponseError};
use actix_web::body::BoxBody;
use serde_json::json;
use crate::cache::Cache;

#[derive(Debug)]
pub enum ErrorType {
    DatabaseError,
    InternalError,
    NotFound,
    BadRequest,
    Unauthorized
}

#[derive(Debug)]
pub struct Error {
    what: String,
    err_type: ErrorType,
}

impl Error {
    pub fn new(what: &str, err_type: ErrorType) -> Self {
        Error{
            what: what.to_string(),
            err_type
        }
    }
}

impl Display for Error {
    fn fmt(&self, f: &mut Formatter<'_>) -> std::fmt::Result {
        write!(f, "{:?}", self)
    }
}

/// actix_web error
impl ResponseError for Error {
    fn status_code(&self) -> StatusCode {
        match self.err_type {
            ErrorType::DatabaseError => StatusCode::INTERNAL_SERVER_ERROR,
            ErrorType::InternalError => StatusCode::INTERNAL_SERVER_ERROR,
            ErrorType::BadRequest => StatusCode::BAD_REQUEST,
            ErrorType::NotFound => StatusCode::NOT_FOUND,
            ErrorType::Unauthorized => StatusCode::UNAUTHORIZED
        }
    }
    fn error_response(&self) -> HttpResponse<BoxBody> {
        let status = self.status_code();
        let message = match status {
            StatusCode::INTERNAL_SERVER_ERROR => "Internal Server Error".to_string(),
            _ => self.what.clone()
        };

        HttpResponse::build(self.status_code())
            .json(json!({ "error": message }))
    }
}

impl From<r2d2::Error> for Error {
    fn from(value: r2d2::Error) -> Self {
        Error::new(&value.to_string(), ErrorType::DatabaseError)
    }
}

impl From<tera::Error> for Error {
    fn from(value: tera::Error) -> Self {
        Error::new(&value.to_string(), ErrorType::InternalError)
    }
}

impl From<PoisonError<MutexGuard<'_, Cache>>> for Error {
    fn from(value: PoisonError<MutexGuard<'_, Cache>>) -> Self {
        Error::new(&value.to_string(), ErrorType::InternalError)
    }
}
