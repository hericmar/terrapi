package config

type Service interface {
	ReceiveConfig(configStr string) error
}

type service struct {
	repository Repository
}

func NewService(r Repository) Service {
	return &service{
		repository: r,
	}
}

// implementation

func (s *service) ReceiveConfig(configStr string) error {
	return nil
}
