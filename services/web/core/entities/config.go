package entities

const (
	Humidity    int = 0
	Temperature     = 1
)

type Sensor struct {
	Name             string `ini:"name" json:"name"`
	Type             string `ini:"type" json:"type"`
	PhysicalQuantity int    `ini:"physical_quantity" json:"physicalQuantity"`
}

type Switch struct {
	Name       string `ini:"-" json:"name"`
	SensorName string `ini:"sensor" json:"sensorName"`
	GPIO       int    `ini:"gpio" json:"gpio"`
}

type Timer struct {
}

type Config struct {
	ClientID   string `json:"clientID"`
	ConfigBody string `json:"-"`

	Sensors  []Sensor `json:"sensors"`
	Switches []Switch `json:"switches"`
}
