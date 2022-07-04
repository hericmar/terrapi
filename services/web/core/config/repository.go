package config

import (
	"errors"
	"sync"
	"terrapi-web/core/entities"
)

var mu sync.RWMutex
var configs map[string]*entities.Config

type Repository interface {
	Create(config *entities.Config) (*entities.Config, error)
	Read(clientID string) (*entities.Config, error)
	Update(config *entities.Config) (*entities.Config, error)
	Delete(clientID string) error
}

type repository struct {
}

func NewRepo() Repository {
	if configs == nil {
		configs = make(map[string]*entities.Config)
	}

	return &repository{}
}

func (r repository) Create(config *entities.Config) (*entities.Config, error) {
	mu.Lock()
	defer mu.Unlock()

	if _, ok := configs[config.ClientID]; ok {
		return nil, errors.New("config already exists")
	}

	configs[config.ClientID] = config

	return config, nil
}

func (r repository) Read(clientID string) (*entities.Config, error) {
	mu.RLock()
	defer mu.RUnlock()

	val, ok := configs[clientID]
	if !ok {
		return nil, errors.New("unknown client with clientID")
	}

	return val, nil
}

func (r repository) Update(config *entities.Config) (*entities.Config, error) {
	//TODO implement me
	panic("implement me")
}

func (r repository) Delete(clientID string) error {
	//TODO implement me
	panic("implement me")
}
