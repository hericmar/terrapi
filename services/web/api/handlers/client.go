package handlers

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/presenter"
	"terrapi-web/core/client"
)

func CreateClient(service client.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		return presenter.DoSuccess(c, fiber.StatusCreated, 1)
	}
}

func DeleteClient(service client.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		return presenter.DoSuccess(c, fiber.StatusNoContent, 1)
	}
}
