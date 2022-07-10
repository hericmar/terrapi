package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/handlers"
	"terrapi-web/api/middleware"
	"terrapi-web/core/config"
)

func ConfigRouter(app fiber.Router, service config.Service, auth middleware.Auth) {
	app.Get("/config/:id", handlers.ReadConfig(service))
	app.Put("/config", handlers.PutConfig(service, auth))
}
