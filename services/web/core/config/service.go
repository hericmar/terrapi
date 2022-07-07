package config

import (
	"errors"
	"terrapi-web/core/entities"
)

type Service interface {
	ReadConfig(clientID string) (*entities.Config, error)
	ReceiveConfig(configStr string) error
}

type service struct {
	repository Repository
}

func NewService(r Repository) Service {
	return &service{
		repository: r,
	}
}

// implementation

func (s *service) ReadConfig(clientID string) (*entities.Config, error) {
	config, err := s.repository.Read(clientID)

	if err != nil {
		return nil, errors.New("config does not exists")
	}

	return config, nil
}

func (s *service) ReceiveConfig(configStr string) error {
	config, err := ParseConfig(configStr)
	if err != nil {
		return err
	}

	_, err = s.repository.Put(config)
	if err != nil {
		return errors.New("invalid config format")
	}

	return nil
}
