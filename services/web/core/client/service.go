package client

import (
	"terrapi-web/core/entities"
	"terrapi-web/utils"
)

type Service interface {
	CreateClient() (*entities.Client, error)
	RemoveClient(clientID string) error
}

type service struct {
	repository Repository
}

func NewService(r Repository) Service {
	return &service{repository: r}
}

func (s service) CreateClient() (*entities.Client, error) {
	var client entities.Client
	client.ID = utils.RandomString(32)

	err := s.repository.Create(&client)

	return &client, err
}

func (s service) RemoveClient(clientID string) error {
	return s.repository.Remove(clientID)
}
