-- 通道信息表
CREATE TABLE IF NOT EXISTS t_channel (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    channel_id VARCHAR(20) UNIQUE NOT NULL,
    device_id VARCHAR(20) NOT NULL,
    channel_name VARCHAR(255),
    manufacturer VARCHAR(100),
    model VARCHAR(100),
    owner VARCHAR(100),
    civil_code VARCHAR(6),
    address VARCHAR(255),
    parental INTEGER DEFAULT 0,
    parent_id VARCHAR(20),
    safety_way INTEGER DEFAULT 0,
    register_way INTEGER DEFAULT 1,
    secrecy INTEGER DEFAULT 0,
    status VARCHAR(10) DEFAULT 'OFF',
    longitude REAL DEFAULT 0,
    latitude REAL DEFAULT 0,
    ptz_type INTEGER DEFAULT 0,
    create_time INTEGER,
    update_time INTEGER,
    FOREIGN KEY (device_id) REFERENCES t_device(device_id)
);

-- 创建索引
CREATE INDEX IF NOT EXISTS idx_channel_device_id ON t_channel(device_id);
CREATE INDEX IF NOT EXISTS idx_channel_status ON t_channel(status);
CREATE INDEX IF NOT EXISTS idx_channel_create_time ON t_channel(create_time);
