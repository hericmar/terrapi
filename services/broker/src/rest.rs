use actix_web::{HttpResponse, web};
use crate::error::Error;
use crate::repo::PostgresPool;

#[derive(Clone, Debug)]
pub struct Context {
    pub db: PostgresPool
}

struct ConfigRequest {
    pub client_id: String,
    pub config: String,
}

/// PUT /api/v1/config
pub async fn put_config(
    ctx: web::Data<Context>, payload: web::Json<ConfigRequest>
) -> Result<HttpResponse, Error> {

}

struct MeasurementRecord {
    pub sensor_name: String,
    pub value: f32,
    pub physical_quantity: i32,
    pub timestamp: u64,
}

struct EventRecord {
    pub switch_name: String,
    pub state: bool,
    pub timestamp: u64,
}

struct RecordsRequest {
    pub client_id: String,
    pub measurements: Vec<MeasurementRecord>,
    pub events: Vec<EventRecord>
}

/// POST /api/v1/records
pub async fn post_records(
    ctx: web::Data<Context>, payload: web::Json<RecordsRequest>
) -> Result<HttpResponse, Error> {

}
