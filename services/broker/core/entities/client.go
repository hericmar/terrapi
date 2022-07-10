package entities

type Client struct {
	ID       string `json:"ID"`
	Password string `json:"password"`
	Name     string `json:"name"`
}
