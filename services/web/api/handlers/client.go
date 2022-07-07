package handlers

import (
	"errors"
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/presenter"
	"terrapi-web/core/client"
)

type ClientRequest struct {
	Name string `json:"name"`
}

func ReadAllClients(service client.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		clients, err := service.ReadAllClients()

		if err != nil {
			return presenter.DoError(
				c,
				fiber.StatusInternalServerError,
				errors.New("database error"))
		}

		return presenter.DoSuccess(c, fiber.StatusOK, clients)
	}
}

func CreateClient(service client.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		var requestBody ClientRequest
		err := c.BodyParser(&requestBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		cl, err := service.CreateClient(requestBody.Name)

		if err != nil {
			return presenter.DoError(
				c,
				fiber.StatusInternalServerError,
				errors.New("database error"))
		}

		return presenter.DoSuccess(c, fiber.StatusCreated, cl)
	}
}

func DeleteClient(service client.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		return presenter.DoSuccess(c, fiber.StatusNoContent, 1)
	}
}
