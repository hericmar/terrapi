use crate::error;

pub type Result<T, E = error::Error> = std::result::Result<T, E>;
