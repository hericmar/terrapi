package measurement

import (
	"terrapi/core/config"
	"terrapi/core/entities"
)

type Service interface {
	ReadAllMeasurement(clientId string, offset int, limit int) ([]entities.Measurement, error)
	ReadMeasurement(clientId string, sensorName string, offset int, limit int) ([]entities.Measurement, error)
	PostMeasurement(measurement *entities.Measurement) error
}

type service struct {
	repository       Repository
	configRepository config.Repository
}

func NewService(r Repository, configRepository config.Repository) Service {
	return &service{
		repository:       r,
		configRepository: configRepository,
	}
}

//

func (s *service) ReadAllMeasurement(clientId string, offset int, limit int) ([]entities.Measurement, error) {
	return s.repository.ReadAll(clientId, offset, limit)
}

func (s *service) ReadMeasurement(clientId string, sensorName string, offset int, limit int) ([]entities.Measurement, error) {
	panic("implement me")

	return nil, nil
}

func (s *service) PostMeasurement(measurement *entities.Measurement) error {
	return s.repository.Create(measurement)
}
