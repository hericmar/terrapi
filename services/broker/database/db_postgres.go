package database

import (
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
	"terrapi/core"
)

func Init(config *core.Config) (*gorm.DB, error) {
	con, err := gorm.Open(postgres.Open(config.DB), &gorm.Config{
		PrepareStmt: true,
		/*
			NamingStrategy: schema.NamingStrategy{
				SingularTable: true,
			}
		*/
	})

	if err != nil {
		return nil, err
	}

	return con, nil
}
