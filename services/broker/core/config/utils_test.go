package config

import "testing"

var configStr string = `
[switch "lights"]
sensor = day
gpio = 17
`

func TestParseConfig(t *testing.T) {
	_, err := ParseConfig(configStr)
	if err != nil {
		t.Errorf("Got error.")
	}
}
