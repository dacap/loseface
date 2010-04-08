CREATE TABLE "users" (
       "id" INTEGER PRIMARY KEY,
       "user_name" VARCHAR,
       "first_name" VARCHAR,
       "last_name" VARCHAR,
       "email" VARCHAR,
       "ssn" VARCHAR
       )

CREATE TABLE "photos" (
       "id" INTEGER PRIMARY KEY,
       "user_id" INTEGER,
       "file_name" TEXT,
       CONSTRAINT fk_user_id FOREIGN KEY (user_id) REFERENCES users (id)
       )
