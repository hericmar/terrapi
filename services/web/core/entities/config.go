package entities

type Sensor struct {
}

type Switch struct {
	Name       string `ini:"-"`
	SensorName string `ini:"sensor"`
	GPIO       int    `ini:"gpio"`
}

type Timer struct {
}

type Config struct {
	ClientID string

	Switches []Switch
}
