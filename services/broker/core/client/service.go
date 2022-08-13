package client

import (
	"terrapi/core/entities"
	"terrapi/utils"
)

type Service interface {
	Read(clientID string) (*entities.Client, error)
	ReadAllClients() ([]entities.Client, error)
	CreateClient(clientName string, password string) (*entities.Client, error)
	RemoveClient(clientID string) error
}

type service struct {
	repository Repository
}

func NewService(r Repository) Service {
	return &service{repository: r}
}

//

func (s service) Read(clientID string) (*entities.Client, error) {
	return s.repository.Read(clientID)
}

func (s service) ReadAllClients() ([]entities.Client, error) {
	return s.repository.ReadAll()
}

func (s service) CreateClient(clientName string, password string) (*entities.Client, error) {
	var client entities.Client
	client.ID = utils.RandomString(32)
	client.Name = clientName
	client.Password = password

	err := s.repository.Create(&client)

	return &client, err
}

func (s service) RemoveClient(clientID string) error {
	return s.repository.Remove(clientID)
}
