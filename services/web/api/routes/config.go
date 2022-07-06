package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/handlers"
	"terrapi-web/core/client"
	"terrapi-web/core/config"
)

func ClientRouter(app fiber.Router, service client.Service) {
	app.Post("/client", handlers.CreateClient(service))
	app.Delete("/delete", handlers.DeleteClient(service))
}

func ConfigRouter(app fiber.Router, service config.Service) {
	app.Put("/config", handlers.PutConfig(service))
}
