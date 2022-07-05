package presenter

import "github.com/gofiber/fiber/v2"

type ConfigRequest struct {
	ClientID   string `json:"clientID"`
	ConfigBody string `json:"config"`
}

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

func SuccessResponse(code int) *fiber.Map {
	return &fiber.Map{
		"status": true,
		"data":   "",
	}
}
