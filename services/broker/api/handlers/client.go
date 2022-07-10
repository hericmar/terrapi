package handlers

import (
	"errors"
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/middleware"
	"terrapi-web/api/presenter"
	"terrapi-web/core/client"
)

type clientRequest struct {
	Name     string `json:"name"`
	Password string `json:"password"`
}

func ReadAllClients(service client.Service, auth middleware.Auth) fiber.Handler {
	return func(c *fiber.Ctx) error {
		if !auth.IsAdminAuthorized(c) {
			return presenter.DoError(c, fiber.StatusUnauthorized, errors.New(""))
		}

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

func CreateClient(service client.Service, auth middleware.Auth) fiber.Handler {
	return func(c *fiber.Ctx) error {
		if !auth.IsAdminAuthorized(c) {
			return presenter.DoError(c, fiber.StatusUnauthorized, errors.New(""))
		}

		var requestBody clientRequest
		err := c.BodyParser(&requestBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		cl, err := service.CreateClient(requestBody.Name, requestBody.Password)

		if err != nil {
			return presenter.DoError(
				c,
				fiber.StatusInternalServerError,
				errors.New("database error"))
		}

		return presenter.DoSuccess(c, fiber.StatusCreated, cl)
	}
}

func DeleteClient(service client.Service, auth middleware.Auth) fiber.Handler {
	return func(c *fiber.Ctx) error {
		if !auth.IsAdminAuthorized(c) {
			return presenter.DoError(c, fiber.StatusUnauthorized, errors.New(""))
		}

		return presenter.DoSuccess(c, fiber.StatusNoContent, 1)
	}
}
