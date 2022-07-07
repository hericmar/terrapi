package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/handlers"
	"terrapi-web/core/client"
)

func ClientRouter(app fiber.Router, service client.Service) {
	app.Get("/client", handlers.ReadAllClients(service))
	app.Post("/client", handlers.CreateClient(service))
	app.Delete("/client", handlers.DeleteClient(service))
}
