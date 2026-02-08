





-- CREATE TABLE t_device (
--     id          INTEGER PRIMARY KEY,
--     deviceid    VARCHAR,
--     devicename        VARCHAR,
--     type        INTEGER,
--      channelcount INTEGER,
--       password    VARCHAR,
--     catalogsubscribe  INTEGER,
--     online        INTEGER,
--      role        VARCHAR
-- );


-- INSERT INTO t_device
-- (deviceid, online,  role) VALUES ('admin', 0, 'ROLE_ADMIN');

  CREATE TABLE t_device (
      id          INTEGER PRIMARY KEY,
      deviceid    VARCHAR UNIQUE,
      devicename        VARCHAR,
    type        INTEGER,
     channelcount INTEGER,
      password    VARCHAR,
    catalogsubscribe  INTEGER,
      online        INTEGER,
    siptransport  INTEGER,
    mediatransport  INTEGER,
     mediatransportmode INTEGER,
     lastregisterat INTEGER,
     lastkeepaliveat INTEGER,
     updatedat INTEGER,
     createdat INTEGER,
	 remoteip   VARCHAR, 
	 remoteport   INTEGER,
     role        VARCHAR
  );