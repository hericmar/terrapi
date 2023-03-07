use std::fmt::{Display, Formatter};
use actix_web::http::StatusCode;
use actix_web::{HttpResponse, ResponseError};
use actix_web::body::BoxBody;

#[derive(Debug)]
pub enum ErrorType {
    DatabaseError,
    InternalError,
    BadRequest,
}

#[derive(Debug)]
pub struct Error {
    what: String,
    err_type: ErrorType,
}

impl Error {
    fn new(what: String, err_type: ErrorType) -> Self {
        Error{what, err_type}
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
        }
    }
    fn error_response(&self) -> HttpResponse<BoxBody> {
        let status = self.status_code();
        let message = match status {
            StatusCode::INTERNAL_SERVER_ERROR => "Internal Server Error".to_string(),
            _ => self.what.clone()
        };

        HttpResponse::build(self.status_code())
            .json(json!( { "error": message } ))
    }
}
