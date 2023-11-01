CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    username TEXT NOT NULL,
    password TEXT NOT NULL,
    species TEXT NOT NULL,
    directAncestors BYTEA ARRAY DEFAULT '{}',
    created_at TIMESTAMP NOT NULL DEFAULT NOW()
);

CREATE TABLE IF NOT EXISTS ancestors (
    id BYTEA PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT NOT NULL,
    species TEXT NOT NULL,
    burialPlace TEXT NOT NULL,
    ownerId INTEGER NOT NULL,
    created_at TIMESTAMP NOT NULL DEFAULT NOW(),
    
    CONSTRAINT fk_owner FOREIGN KEY(ownerId) REFERENCES users(id) ON DELETE CASCADE
);

