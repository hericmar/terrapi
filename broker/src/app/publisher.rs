use actix_web::{HttpResponse, web};
use serde::{Deserialize, Serialize};
use crate::prelude::*;
use crate::app::AppState;
use crate::db;
use crate::error::Error;
use crate::models::publisher::Publisher;

#[derive(Deserialize)]
pub struct CreatePublisher {
    pub name: String,
}

type UpdatePublisherRequest = CreatePublisher;

#[derive(Serialize)]
struct PublisherResponse {
    client_id: String,
    name: String,
}

type PublisherDetailResponse = Publisher;

#[derive(Deserialize)]
pub struct PublisherPath {
    client_id: String,
}

/// POST /api/v1/publishers
pub async fn create(
    ctx: web::Data<AppState>, payload: web::Json<CreatePublisher>
) -> Result<HttpResponse, Error> {
    let result = db::publisher::create(&mut ctx.db.get()?, &payload.into_inner())?;
    Ok(HttpResponse::Ok().json(result))
}

/// GET /api/v1/publishers
pub async fn read_all(
    ctx: web::Data<AppState>
) -> Result<HttpResponse> {
    let result = db::publisher::read_all(&mut ctx.db.get()?)?;
    Ok(HttpResponse::Ok().json(result))
}

/// PATCH /api/v1/publishers/{client_id}
pub async fn update(
    ctx: web::Data<AppState>,
    path: web::Path<PublisherPath>,
    payload: web::Json<UpdatePublisherRequest>
) -> Result<HttpResponse> {
    let data = &db::publisher::UpdatePublisher{
        name: Some(&payload.name),
        config: None,
    };
    let result = db::publisher::update(&mut ctx.db.get()?, &path.client_id, data)?;
    Ok(HttpResponse::Ok().json(result))
}

/// DELETE /api/v1/publishers/{client_id}
pub async fn delete(
    ctx: web::Data<AppState>,
    path: web::Path<PublisherPath>
) -> Result<HttpResponse> {
    let result = db::publisher::delete(&mut ctx.db.get()?, &path.client_id)?;
    Ok(HttpResponse::Ok().json(result))
}

#[cfg(test)]
mod tests {
    use actix_web::{App, test, web};
    use crate::app::AppState;
    use crate::app::publisher::create;
    use crate::models::publisher::Publisher;

    #[actix_web::test]
    async fn test_create_publisher() {
        let app = test::init_service(
            App::new()
                .app_data(web::Data::new(AppState{
                    db: crate::db::new_pool("postgres://terrapi:password@localhost:5432/terrapi")
                        .expect("cannot create database pool"),
                }))
                .route("/api/v1/publisher", web::post().to(create))
        ).await;

        let request = test::TestRequest::post()
            .uri("/api/v1/publisher")
            .set_json(&serde_json::json!({
                "name": "test",
            }))
            .to_request();

        let response = test::call_service(&app, request).await;
        // println!("{:?}", test::read_body(response).await);
        assert!(response.status().is_success());

        let result: Publisher = test::read_body_json(response).await;
        assert_eq!(result.name, "test");
    }
}
