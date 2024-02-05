type State = i32;

const STATE_OFF: State = 0;
const STATE_ON: State = 1;

pub struct Event {
    pub timestamp: u64,
    pub client_id: String,
    pub device_id: String,
    pub state: State
}

pub struct Measurement {
    pub timestamp: u64,
    pub client_id: String,
    pub sensor_id: String,
    pub value: f32
}
