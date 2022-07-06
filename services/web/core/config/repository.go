package config

import (
	"errors"
	"sync"
	"terrapi-web/core/entities"
)

var mu sync.RWMutex
var configs map[string]*entities.Config

type Config struct {
	ClientID   string
	ConfigBody string
}

type Repository interface {
	Read(clientID string) (*entities.Config, error)
	Put(config *entities.Config) (*entities.Config, error)
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

func (r *repository) Put(config *entities.Config) (*entities.Config, error) {
	mu.Lock()
	defer mu.Unlock()

	configs[config.ClientID] = config

	return config, nil
}

func (r *repository) Read(clientID string) (*entities.Config, error) {
	mu.RLock()
	defer mu.RUnlock()

	val, ok := configs[clientID]
	if !ok {
		return nil, errors.New("unknown client with clientID")
	}

	return val, nil
}

func (r *repository) Delete(clientID string) error {
	//TODO implement me
	panic("implement me")
}
