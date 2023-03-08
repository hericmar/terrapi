use chrono::{Local, Offset, TimeZone};

pub fn local_tz_offset() -> i32 {
    Local.timestamp_opt(0, 0).unwrap().offset().fix().local_minus_utc()
}

