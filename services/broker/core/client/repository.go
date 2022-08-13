package client

import (
	"fmt"
	"gorm.io/gorm"
	"terrapi/core/entities"
)

type Repository interface {
	Read(ID string) (*entities.Client, error)
	ReadAll() ([]entities.Client, error)
	Create(client *entities.Client) error
	Remove(clientID string) error
}

type client struct {
	ID       string `gorm:"primaryKey;index"`
	Password string `gorm:"notNull"`
	Name     string `gorm:"notNull"`
}

type repository struct {
	db *gorm.DB
}

func NewRepo(db *gorm.DB) Repository {
	if err := db.AutoMigrate(&client{}); err != nil {
		fmt.Println(err)
	}

	return &repository{db}
}

func mapping(c *entities.Client) *client {
	return &client{
		ID:       c.ID,
		Name:     c.Name,
		Password: c.Password,
	}
}

//

func (r *repository) Read(ID string) (*entities.Client, error) {
	var client entities.Client

	err := r.db.First(&client, "id = ?", ID).Error

	return &client, err
}

func (r *repository) ReadAll() ([]entities.Client, error) {
	var clients []entities.Client

	err := r.db.Find(&clients).Error

	return clients, err
}

func (r *repository) Create(client *entities.Client) error {
	model := mapping(client)

	return r.db.Create(model).Error
}

func (r *repository) Remove(clientID string) error {
	return r.db.Delete(&client{}, clientID).Error
}
