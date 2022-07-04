package config

import "testing"

var config string = `
[switch "lights"]
sensor = day
gpio = 17
`

func TestParseConfig(t *testing.T) {
	_, err := ParseConfig(config)
	if err != nil {
		t.Errorf("Got error.")
	}
}
