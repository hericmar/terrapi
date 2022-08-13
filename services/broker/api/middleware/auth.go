package middleware

import (
	"github.com/gofiber/fiber/v2"
	"terrapi/core"
	"terrapi/core/client"
)

type Auth interface {
	IsAdminAuthorized(c *fiber.Ctx) bool
	IsAuthorized(c *fiber.Ctx, clientID string) bool
}

type auth struct {
	config        *core.Config
	clientService client.Service
}

func NewAuth(config *core.Config, clientService client.Service) Auth {
	return &auth{config, clientService}
}

//

func (a *auth) IsAdminAuthorized(c *fiber.Ctx) bool {
	if val, ok := c.GetReqHeaders()["Authorization"]; ok {
		return a.config.AdminPassword == val
	}

	return false
}

func (a *auth) IsAuthorized(c *fiber.Ctx, clientID string) bool {
	if val, ok := c.GetReqHeaders()["Authorization"]; ok {
		cli, err := a.clientService.Read(clientID)
		if err != nil {
			return false
		}

		return cli.Password == val
	}

	return false
}
