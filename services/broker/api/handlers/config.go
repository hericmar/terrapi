package handlers

import (
	"errors"
	"github.com/gofiber/fiber/v2"
	"terrapi/api/middleware"
	"terrapi/api/presenter"
	"terrapi/core/config"
)

type configRequest struct {
	ClientID   string `json:"clientID"`
	ConfigBody string `json:"config"`
}

func ReadAllConfigs(service config.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		configs, err := service.ReadAllConfigs()
		if err != nil {
			return presenter.DoError(c, fiber.StatusInternalServerError, errors.New("database error"))
		}

		return presenter.DoSuccess(c, fiber.StatusOK, configs)
	}
}

func ReadConfig(service config.Service) fiber.Handler {
	return func(c *fiber.Ctx) error {
		param := struct {
			ID string `params:"id"`
		}{}
		err := c.ParamsParser(&param)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		conf, err := service.ReadConfig(param.ID)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		return presenter.DoSuccess(c, fiber.StatusOK, conf)
	}
}

func PutConfig(service config.Service, auth middleware.Auth) fiber.Handler {
	return func(c *fiber.Ctx) error {
		var requestBody configRequest
		err := c.BodyParser(&requestBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		if !auth.IsAuthorized(c, requestBody.ClientID) {
			return presenter.DoError(c, fiber.StatusUnauthorized, errors.New(""))
		}

		err = service.ReceiveConfig(requestBody.ConfigBody)
		if err != nil {
			return presenter.DoError(c, fiber.StatusBadRequest, err)
		}

		return presenter.DoSuccess(c, fiber.StatusCreated, 1)
	}
}
