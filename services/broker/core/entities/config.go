package entities

// Must be same in the C++ source.
const (
	Humidity    int = 0
	Temperature     = 1
)

type Sensor struct {
	Name string `ini:"name" json:"name"`
	Type string `ini:"type" json:"type"`
}

type Switch struct {
	Name string `ini:"-" json:"name"`
	GPIO int    `ini:"gpio" json:"gpio"`
}

type Timer struct {
}

type Config struct {
	ClientID   string `json:"clientID"`
	ClientName string `json:"name"`
	ConfigBody string `json:"-"`

	Sensors  []Sensor `json:"sensors"`
	Switches []Switch `json:"switches"`
}
