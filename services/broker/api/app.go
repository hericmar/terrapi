package main

import (
	"fmt"
	"github.com/gofiber/fiber/v2"
	"log"
	"os"
	"terrapi-web/api/middleware"
	"terrapi-web/api/routes"
	"terrapi-web/core"
	"terrapi-web/core/client"
	"terrapi-web/core/config"
	"terrapi-web/core/measurement"
	"terrapi-web/database"
)

func main() {
	cfg, err := core.ParseConfig("env")
	if err != nil {
		fmt.Println("Cannot load configuration.")
		os.Exit(1)
	}

	db, err := database.Init(cfg)
	if err != nil {
		fmt.Println("Cannot connect to the database.")
		os.Exit(1)
	}

	clientRepo := client.NewRepo(db)
	configRepo := config.NewRepo(db)
	measurementRepo := measurement.NewRepo(db)

	clientService := client.NewService(clientRepo)
	configService := config.NewService(configRepo)
	measurementService := measurement.NewService(measurementRepo, configRepo)

	auth := middleware.NewAuth(cfg, clientService)

	app := fiber.New()
	api := app.Group("/api")
	routes.ClientRouter(api, clientService, auth)
	routes.ConfigRouter(api, configService, auth)
	routes.MeasurementRouter(api, measurementService, auth)

	log.Fatal(app.Listen(":8080"))
}
