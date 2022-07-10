package entities

import "time"

type Measurement struct {
	ClientID   string    `json:"clientID"`
	SensorName string    `json:"sensorName"`
	Value      string    `json:"value"`
	Timestamp  time.Time `json:"timestamp"`
}
