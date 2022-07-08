package measurement

import (
	"errors"
	"fmt"
	"gorm.io/gorm"
	"terrapi-web/core/entities"
	"time"
)

type Repository interface {
	ReadAll(clientId string, offset int, limit int) ([]entities.Measurement, error)
	Read(clientId string, sensorName string, offset int, limit int) ([]entities.Measurement, error)
	Create(measurement *entities.Measurement) error
}

type measurement struct {
	ID         int       `gorm:"primaryKey;autoIncrement"`
	ClientID   string    `gorm:"notNull"`
	SensorName string    `gorm:"index"`
	Value      string    `gorm:"notNull"`
	Timestamp  time.Time `gorm:"notNull"`
}

type repository struct {
	db *gorm.DB
}

func NewRepo(db *gorm.DB) Repository {
	if err := db.AutoMigrate(&measurement{}); err != nil {
		fmt.Println(err)
	}

	return &repository{db}
}

func mapping(m *entities.Measurement) *measurement {
	return &measurement{
		ClientID:   m.ClientID,
		SensorName: m.SensorName,
		Value:      m.Value,
		Timestamp:  m.Timestamp,
	}
}

func convert(m *measurement) *entities.Measurement {
	return &entities.Measurement{
		ClientID:   m.ClientID,
		SensorName: m.SensorName,
		Value:      m.Value,
		Timestamp:  m.Timestamp,
	}
}

//

func (r *repository) ReadAll(clientId string, offset int, limit int) ([]entities.Measurement, error) {
	var entries []measurement
	var result []entities.Measurement

	err := r.db.Limit(limit).Offset(offset).Where("client_id = ?", clientId).Find(&entries).Error
	if err != nil {
		return nil, errors.New("database error")
	}

	for _, entry := range entries {
		result = append(result, *convert(&entry))
	}

	return result, nil
}

func (r *repository) Read(clientId string, sensorName string, offset int, limit int) ([]entities.Measurement, error) {
	var entries []measurement
	var result []entities.Measurement

	err := r.db.Limit(limit).Offset(offset).Where("client_id = ? AND sensor_name = ?", clientId, sensorName).Find(&entries).Error
	if err != nil {
		return nil, errors.New("database error")
	}

	for _, entry := range entries {
		result = append(result, *convert(&entry))
	}

	return result, nil
}

func (r *repository) Create(measurement *entities.Measurement) error {
	model := mapping(measurement)

	return r.db.Create(model).Error
}
