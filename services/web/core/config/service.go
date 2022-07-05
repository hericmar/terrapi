package config

import "errors"

type Service interface {
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
