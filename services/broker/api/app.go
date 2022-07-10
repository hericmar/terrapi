package main

import (
	"fmt"
	"github.com/gofiber/fiber/v2"
	"github.com/gofiber/fiber/v2/middleware/cors"
	"github.com/gofiber/fiber/v2/middleware/logger"
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
	configService := config.NewService(configRepo, clientRepo)
	measurementService := measurement.NewService(measurementRepo, configRepo)

	auth := middleware.NewAuth(cfg, clientService)

	app := fiber.New()

	app.Use(cors.New())

	/*
		// app.Use(csrf.New())
		// Or extend your config for customization
		app.Use(csrf.New(csrf.Config{
			KeyLookup:      "header:X-Csrf-Token",
			CookieName:     "csrf_",
			CookieSameSite: "Strict",
			Expiration:     1 * time.Hour,
			KeyGenerator:   utils.UUID,
		}))
	*/

	app.Use(logger.New())

	api := app.Group("/api")
	routes.ClientRouter(api, clientService, auth)
	routes.ConfigRouter(api, configService, auth)
	routes.MeasurementRouter(api, measurementService, auth)

	log.Fatal(app.Listen(":8000"))
}
