package core

import (
	"fmt"
	"github.com/caarlos0/env/v6"
	"github.com/joho/godotenv"
)

type Config struct {
	DB            string `env:"DB"`
	Port          int    `env:"PORT"`
	AdminPassword string `env:"ADMIN_PASSWORD"`
}

func ParseConfig(path string) (*Config, error) {
	config := Config{}

	if err := godotenv.Load(path); err != nil {
		fmt.Println(err)
		return nil, err
	}

	if err := env.Parse(&config); err != nil {
		fmt.Printf("%+v\n", err)
		return nil, err
	}

	return &config, nil
}
