package database

import (
	"errors"
	"gorm.io/gorm"
	"terrapi-web/core"
)

func ToError(gormError error) core.Error {
	if errors.Is(gormError, gorm.ErrRecordNotFound) {
		return core.NewErr(core.ErrNotFound, gormError.Error())
	}

	return core.NewErr(core.ErrInternal, gormError.Error())
}
