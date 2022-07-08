package config

import (
	"errors"
	"gopkg.in/ini.v1"
	"regexp"
	"strings"
	"terrapi-web/core/entities"
)

func GetSectionName(sectionHeader string) (string, error) {
	re := regexp.MustCompile(" \"([0-9a-z.]+)")
	matches := re.FindStringSubmatch(sectionHeader)

	if len(matches) != 2 {
		return "", errors.New("cannot match section name")
	}

	return matches[1], nil
}

func ParseConfig(configStr string) (*entities.Config, error) {
	cfg, err := ini.Load([]byte(configStr))

	var config = &entities.Config{}
	config.ConfigBody = configStr

	if err != nil {
		return nil, err
	}

	if section, _ := cfg.GetSection("general"); section != nil {
		clientID, err := section.GetKey("client_id")
		if err != nil {
			return nil, errors.New("\"client_id\" is empty")
		}
		config.ClientID = clientID.Value()
	} else {
		return nil, errors.New("missing [general] section")
	}

	var sections = cfg.SectionStrings()

	for _, section := range sections {
		if strings.HasPrefix(section, "sensor") {
			s := entities.Sensor{}

			err := cfg.Section(section).MapTo(&s)
			if err != nil {
				continue
			}

			name, err := GetSectionName(section)
			if err == nil {
				s.Name = name

				physQuantity, err := cfg.Section(section).GetKey("physical_quantity")
				if err != nil {
					return nil, errors.New("\"physical_quantity\" is empty")
				}

				switch physQuantity.Value() {
				case "humidity":
					s.PhysicalQuantity = entities.Humidity
				case "temperature":
					s.PhysicalQuantity = entities.Temperature
				}

				config.Sensors = append(config.Sensors, s)
			}
		}

		if strings.HasPrefix(section, "switch") {
			s := entities.Switch{}

			err := cfg.Section(section).MapTo(&s)
			if err != nil {
				continue
			}

			name, err := GetSectionName(section)
			if err == nil {
				s.Name = name
				config.Switches = append(config.Switches, s)
			}
		}
	}

	return config, nil
}
