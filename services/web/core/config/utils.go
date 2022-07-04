package config

import (
	"gopkg.in/ini.v1"
	"regexp"
	"strings"
	"terrapi-web/core/entities"
)

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

	var sections = cfg.SectionStrings()

	for _, section := range sections {
		if strings.HasPrefix(section, "switch") {
			s := entities.Switch{}
			err := cfg.Section(section).MapTo(&s)

			if err != nil {
				continue
			}

			re := regexp.MustCompile(" \"([a-z]+)")
			matches := re.FindStringSubmatch(section)

			if err == nil && len(matches) == 2 {
				s.Name = matches[1]
				config.Switches = append(config.Switches, s)
			}
		}
	}

	return config, nil
}
