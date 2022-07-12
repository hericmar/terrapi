package core

const (
	ErrNotFound int = 0
	ErrUnauthorized
	ErrInternal
)

type Error struct {
	Type        int
	description string
}

func NewErr(t int, description string) Error {
	return Error{t, description}
}

func (e *Error) Error() string {
	return e.description
}
