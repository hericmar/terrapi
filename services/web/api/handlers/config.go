package handlers

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/presenter"
	"terrapi-web/core/config"
)

func PutConfig(service config.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		var requestBody presenter.ConfigRequest
		err := c.BodyParser(&requestBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		err = service.ReceiveConfig(requestBody.ConfigBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		return presenter.DoSuccess(c, fiber.StatusCreated, 1)
	}
}
