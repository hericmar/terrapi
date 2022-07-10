package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi-web/api/handlers"
	"terrapi-web/api/middleware"
	"terrapi-web/core/measurement"
)

func MeasurementRouter(app fiber.Router, service measurement.Service, auth middleware.Auth) {
	app.Get("/measurement/:clientID", handlers.ReadAllMeasurement(service))
	app.Get("/measurement/:clientID/:sensorName", handlers.ReadMeasurement(service))
	app.Post("/measurement/:clientID/:sensorName", handlers.PostMeasurement(service, auth))
}
