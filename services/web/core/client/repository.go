package client

import (
	"gorm.io/gorm"
	"terrapi-web/core/entities"
)

type Repository interface {
	Create(client *entities.Client) error
	Remove(clientID string) error
}

type client struct {
	ID string `gorm:"primaryKey;index"`
}

type repository struct {
	db *gorm.DB
}

func NewRepo(db *gorm.DB) Repository {
	return &repository{db}
}

func mapping(c *entities.Client) *client {
	return &client{
		ID: c.ID,
	}
}

func (r *repository) Create(client *entities.Client) error {
	model := mapping(client)

	return r.db.Create(model).Error
}

func (r *repository) Remove(clientID string) error {
	return r.db.Delete(&client{}, clientID).Error
}
