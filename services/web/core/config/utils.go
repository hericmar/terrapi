package config

import (
	"errors"
	"gopkg.in/ini.v1"
	"regexp"
	"strings"
	"terrapi-web/core/entities"
)

func GetSectionName(sectionHeader string) (string, error) {
	re := regexp.MustCompile(" \"([a-z]+)")
	matches := re.FindStringSubmatch(sectionHeader)

	if len(matches) != 2 {
		return "", errors.New("cannot match section name")
	}

	return matches[1], nil
}

func ParseConfig(configStr string) (*entities.Config, error) {
	cfg, err := ini.Load([]byte(configStr))

	var config = &entities.Config{}

	if err != nil {
		return nil, err
	}

	/*
		section, err := cfg.GetSection("general")
		if err != nil {
			return nil, errors.New("missing [general] section")
		}
	*/

	clientID, err := cfg.Section("DEFAULT").GetKey("clientID")
	if err != nil {
		return nil, errors.New("clientID is empty")
	}
	config.ClientID = clientID.Value()

	var sections = cfg.SectionStrings()

	for _, section := range sections {
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
