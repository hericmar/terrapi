package main

import (
	"github.com/gofiber/fiber/v2"
	"log"
	"terrapi-web/api/routes"
	"terrapi-web/core/config"
)

func main() {
	configRepo := config.NewRepo()
	configService := config.NewService(configRepo)

	app := fiber.New()
	api := app.Group("/api")
	routes.ConfigRouter(api, configService)

	log.Fatal(app.Listen(":8080"))
}
