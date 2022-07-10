package presenter

import "github.com/gofiber/fiber/v2"

func DoError(c *fiber.Ctx, httpCode int, err error) error {
	return c.Status(httpCode).JSON(fiber.Map{
		"status":  "error",
		"message": err.Error(),
		"data":    nil,
	})
}

func DoSuccess(c *fiber.Ctx, httpCode int, data interface{}) error {
	return c.Status(httpCode).JSON(fiber.Map{
		"status":  "success",
		"message": nil,
		"data":    data,
	})
}
