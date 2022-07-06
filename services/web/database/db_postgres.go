package database

import (
	"gorm.io/driver/postgres"
	"gorm.io/gorm"
)

func Init() (*gorm.DB, error) {
	con, err := gorm.Open(postgres.Open(""), &gorm.Config{
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
