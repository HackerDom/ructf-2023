CREATE TABLE nodes(
    dev        INT, 
    ino        SERIAL PRIMARY KEY, 
    nlink      INT,
    mode       INT,         
    uid        INT,        
    gid        INT,       
    rdev       INT,
    size       INT,
    blksize    INT,
    blocks     INT,
    atime_sec  BIGINT,      
    atime_nsec BIGINT,        
    mtime_sec  BIGINT,   
    mtime_nsec BIGINT,          
    ctime_sec  BIGINT,         
    ctime_nsec BIGINT      
);

CREATE TABLE entries(
    ino      INT,
    path     TEXT,
    filename TEXT,

    PRIMARY KEY(path, filename) 
);
CREATE INDEX path_index ON entries(path);

CREATE TABLE users(
    first_name TEXT,
    last_name  TEXT,
    age        INT,
    email      TEXT,
    ino        INT,
    private    BOOLEAN
);

CREATE TABLE debug (
    msg TEXT
);

CREATE FUNCTION log (TEXT) RETURNS BOOLEAN AS $$
DECLARE
BEGIN
    INSERT INTO debug VALUES ($1);
    RETURN true;
END;
$$ LANGUAGE plpgsql COST 0.0000000000000000000000000000000000000000001;
