use actix_web::{HttpResponse, web};
use actix_web::http::StatusCode;
use crate::app::AppState;
use crate::db;
use crate::prelude::*;

struct Hello {
    client_id: String,
    config: String
}

/// PUT /api/v1/hello
pub fn hello(
    ctx: web::Data<AppState>, payload: web::Json<Hello>
) -> Result<HttpResponse> {
    let data = &db::publisher::UpdatePublisher{
        name: None,
        config: Some(payload.config.clone()),
    };
    db::publisher::update(&mut ctx.db.get()?, &payload.client_id, &data)?;

    Ok(HttpResponse::Ok().status(StatusCode::NO_CONTENT).finish())
}
