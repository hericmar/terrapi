package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi/api/handlers"
	"terrapi/api/middleware"
	"terrapi/core/config"
)

func ConfigRouter(app fiber.Router, service config.Service, auth middleware.Auth) {
	app.Get("/config", handlers.ReadAllConfigs(service))
	app.Get("/config/:id", handlers.ReadConfig(service))
	app.Put("/config", handlers.PutConfig(service, auth))
}
