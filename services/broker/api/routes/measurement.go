package routes

import (
	"github.com/gofiber/fiber/v2"
	"terrapi/api/handlers"
	"terrapi/api/middleware"
	"terrapi/core/measurement"
)

func MeasurementRouter(app fiber.Router, service measurement.Service, auth middleware.Auth) {
	app.Get("/measurement/:clientID", handlers.ReadAllMeasurement(service))
	app.Post("/measurement/:clientID", handlers.PostMeasurement(service, auth))
	app.Get("/measurement/:clientID/:sensorName", handlers.ReadMeasurement(service))
}
