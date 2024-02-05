-- This file should undo anything in `up.sql`
ALTER TABLE "publishers" DROP COLUMN created_at;

DROP TABLE IF EXISTS "records";
DROP TABLE IF EXISTS "events";
DROP TABLE IF EXISTS "devices";
DROP TABLE IF EXISTS "measurements";
