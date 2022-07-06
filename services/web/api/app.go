package main

import (
	"fmt"
	"github.com/gofiber/fiber/v2"
	"log"
	"os"
	"terrapi-web/api/routes"
	"terrapi-web/core/client"
	"terrapi-web/core/config"
	"terrapi-web/database"
)

func main() {
	db, err := database.Init()
	if err != nil {
		fmt.Println("Cannot connect to the database.")
		os.Exit(1)
	}

	clientRepo := client.NewRepo(db)
	configRepo := config.NewRepo()

	clientService := client.NewService(clientRepo)
	configService := config.NewService(configRepo)

	app := fiber.New()
	api := app.Group("/api")
	routes.ClientRouter(api, clientService)
	routes.ConfigRouter(api, configService)

	log.Fatal(app.Listen(":8080"))
}
