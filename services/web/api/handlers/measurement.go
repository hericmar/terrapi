package handlers

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/presenter"
	"terrapi-web/core/entities"
	"terrapi-web/core/measurement"
	"time"
)

type measurementRequest struct {
	Value     string    `json:"value"`
	Timestamp time.Time `json:"timestamp"`
}

func ReadAllMeasurement(service measurement.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		params := struct {
			ClientID string `params:"clientID"`
		}{}
		err := c.ParamsParser(&params)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		query := struct {
			Offset int `params:"offset"`
			Limit  int `params:"limit"`
		}{}
		if err := c.QueryParser(&query); err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		result, err := service.ReadAllMeasurement(params.ClientID, query.Offset, query.Limit)

		if err != nil {
			return presenter.DoError(c, fiber.StatusInternalServerError, err)
		}

		for _, r := range result {
			r.Timestamp = time.Now()
		}

		return presenter.DoSuccess(c, fiber.StatusOK, result)
	}
}

func ReadMeasurement(service measurement.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		params := struct {
			ClientID   string `params:"clientID"`
			SensorName string `params:"sensorName"`
		}{}
		err := c.ParamsParser(&params)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		query := struct {
			Offset int `params:"offset"`
			Limit  int `params:"limit"`
		}{}
		if err := c.QueryParser(query); err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		result, err := service.ReadMeasurement(params.ClientID, params.SensorName, query.Offset, query.Limit)

		if err != nil {
			return presenter.DoError(c, fiber.StatusInternalServerError, err)
		}

		return presenter.DoSuccess(c, fiber.StatusOK, result)
	}
}

func PostMeasurement(service measurement.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		params := struct {
			ClientID   string `params:"clientID"`
			SensorName string `params:"sensorName"`
		}{}

		err := c.ParamsParser(&params)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		var requestBody measurementRequest
		err = c.BodyParser(&requestBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		m := entities.Measurement{
			ClientID:   params.ClientID,
			SensorName: params.SensorName,
			Value:      requestBody.Value,
			Timestamp:  requestBody.Timestamp,
		}
		err = service.PostMeasurement(&m)

		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		return presenter.DoSuccess(c, fiber.StatusOK, 1)
	}
}
