package entities

import "time"

type Measurement struct {
	ClientID         string    `json:"clientID"`
	SensorName       string    `json:"sensorName"`
	Value            string    `json:"value"`
	PhysicalQuantity int       `json:"physicalQuantity"`
	Timestamp        time.Time `json:"timestamp"`
}
