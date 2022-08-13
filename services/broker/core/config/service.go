package config

import (
	"errors"
	"terrapi/core/client"
	"terrapi/core/entities"
)

type Service interface {
	ReadConfig(clientID string) (*entities.Config, error)
	ReadAllConfigs() ([]entities.Config, error)
	ReceiveConfig(configStr string) error
}

type service struct {
	repository       Repository
	clientRepository client.Repository
}

func NewService(r Repository, clientRepository client.Repository) Service {
	return &service{
		repository:       r,
		clientRepository: clientRepository,
	}
}

// implementation

func fillClientName(s *service, config *entities.Config) {
	cli, err := s.clientRepository.Read(config.ClientID)
	if err != nil {
		return
	}

	config.ClientName = cli.Name
}

func (s *service) ReadConfig(clientID string) (*entities.Config, error) {
	config, err := s.repository.Read(clientID)

	if err != nil {
		return nil, errors.New("config does not exists")
	}

	fillClientName(s, config)

	return config, nil
}

func (s *service) ReadAllConfigs() ([]entities.Config, error) {
	result, err := s.repository.ReadAll()

	if err != nil {
		return nil, err
	}

	for i, _ := range result {
		fillClientName(s, &result[i])
	}

	return result, nil
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
