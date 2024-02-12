use actix_identity::error::LoginError;
use actix_identity::Identity;
use actix_web::{HttpMessage, HttpRequest, HttpResponse, web};
use serde::Deserialize;
use crate::app::AppState;
use crate::error::{Error, ErrorType};
use crate::prelude::*;

#[derive(Deserialize)]
pub struct LoginRequest {
    pub password: String,
}

impl From<LoginError> for Error {
    fn from(value: LoginError) -> Self {
        Error::new("invalid cookie", ErrorType::Unauthorized)
    }
}

pub async fn login(
    ctx: web::Data<AppState>,
    req: HttpRequest,
    payload: web::Json<LoginRequest>
) -> Result<HttpResponse> {
    if payload.password == ctx.admin_password {
        Identity::login(&req.extensions(), "Admin".into())?;
        Ok(HttpResponse::Ok().finish())
    } else {
        Err(Error::new("unauthorized", ErrorType::Unauthorized))
    }
}

pub async fn user(
    user: Identity
) -> Result<HttpResponse> {
    Ok(HttpResponse::Ok().finish())
}

pub async fn logout(
    user: Identity
) -> Result<HttpResponse> {
    user.logout();
    Ok(HttpResponse::Ok().finish())
}
