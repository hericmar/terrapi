package config

import (
	"errors"
	"fmt"
	"gorm.io/gorm"
	"sync"
	"terrapi-web/core/entities"
)

var mu sync.RWMutex
var configs map[string]*entities.Config

type config struct {
	ClientID   string `gorm:"primaryKey;index"`
	ConfigBody string `gorm:"notNull"`
}

type Repository interface {
	Read(clientID string) (*entities.Config, error)
	Put(config *entities.Config) (*entities.Config, error)
	Delete(clientID string) error
}

type repository struct {
	db *gorm.DB
}

func NewRepo(db *gorm.DB) Repository {
	if err := db.AutoMigrate(&config{}); err != nil {
		fmt.Println(err)
	}

	if configs == nil {
		configs = make(map[string]*entities.Config)
	}

	repo := &repository{db}

	// read all from DB
	var rawConfigs []config
	err := db.Find(&rawConfigs).Error
	if err != nil {
		panic("cannot read \"config\" from the database")
	}

	mu.Lock()
	defer mu.Unlock()
	for _, rawConfig := range rawConfigs {
		// config should be valid once it is in the database
		config, err := ParseConfig(rawConfig.ConfigBody)
		if err != nil {
			panic("cannot parse \"config\" from the database")
		}

		configs[rawConfig.ClientID] = config
	}

	return repo
}

func mapping(c *entities.Config) *config {
	return &config{ClientID: c.ClientID, ConfigBody: c.ConfigBody}
}

//

func (r *repository) Read(clientID string) (*entities.Config, error) {
	mu.RLock()
	defer mu.RUnlock()

	val, ok := configs[clientID]
	if !ok {
		return nil, errors.New("unknown client with clientID")
	}

	return val, nil
}

func (r *repository) Put(config *entities.Config) (*entities.Config, error) {
	mu.Lock()
	defer mu.Unlock()

	configs[config.ClientID] = config

	dbConfig := mapping(config)
	if err := r.db.First(dbConfig).Error; err == nil {
		// exists, update
		dbConfig.ConfigBody = config.ConfigBody
		if err := r.db.Save(dbConfig).Error; err != nil {
			return nil, errors.New("database error")
		}
	} else {
		// does not exist, create new
		if err := r.db.Create(dbConfig).Error; err != nil {
			return nil, errors.New("database error")
		}
	}

	return config, nil
}

func (r *repository) Delete(clientID string) error {
	//TODO implement me
	panic("implement me")
}
