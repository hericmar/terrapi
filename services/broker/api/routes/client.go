package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/handlers"
	"terrapi-web/api/middleware"
	"terrapi-web/core/client"
)

func ClientRouter(app fiber.Router, service client.Service, auth middleware.Auth) {
	app.Get("/client", handlers.ReadAllClients(service, auth))
	app.Post("/client", handlers.CreateClient(service, auth))
	app.Delete("/client", handlers.DeleteClient(service, auth))
}
