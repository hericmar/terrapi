use std::fmt::{Display, Formatter};
use actix_web::http::StatusCode;
use actix_web::{HttpResponse, ResponseError};
use actix_web::body::BoxBody;
use serde_json::json;
use log::log;
use validator::ValidationErrors;

#[derive(Debug, Clone)]
pub enum ErrorType {
    DatabaseError,
    InternalError,
    NotFound,
    BadRequest,
    Unauthorized
}

#[derive(Debug, Clone)]
pub struct Error {
    what: String,
    pub err_type: ErrorType,
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
            StatusCode::INTERNAL_SERVER_ERROR => {
                log::error!("{}: {}", status, self.what);

                "Internal Server Error".to_string()
            },
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

impl From<ValidationErrors> for Error {
    fn from(errors: ValidationErrors) -> Self {
        use serde_json::{Map as JsonMap, Value as JsonValue};

        let mut err_map = JsonMap::new();

        // transforms errors into objects that err_map can take
        for (field, errors) in errors.field_errors().iter() {
            let errors: Vec<JsonValue> = errors
                .iter()
                .map(|error| {
                    // dbg!(error) // <- Uncomment this if you want to see what error looks like
                    json!(error.message)
                })
                .collect();
            err_map.insert(field.to_string(), json!(errors));
        }

        let err: String = err_map.iter().map(|(k, v)| format!("{}: {}", k, v)).collect();

        Error::new(&err, ErrorType::BadRequest)
    }
}
