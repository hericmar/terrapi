package client

import (
	"terrapi-web/core/entities"
	"terrapi-web/utils"
)

type Service interface {
	ReadAllClients() ([]entities.Client, error)
	CreateClient(clientName string) (*entities.Client, error)
	RemoveClient(clientID string) error
}

type service struct {
	repository Repository
}

func NewService(r Repository) Service {
	return &service{repository: r}
}

func (s service) ReadAllClients() ([]entities.Client, error) {
	return s.repository.ReadAll()
}

func (s service) CreateClient(clientName string) (*entities.Client, error) {
	var client entities.Client
	client.ID = utils.RandomString(32)
	client.Name = clientName

	err := s.repository.Create(&client)

	return &client, err
}

func (s service) RemoveClient(clientID string) error {
	return s.repository.Remove(clientID)
}
