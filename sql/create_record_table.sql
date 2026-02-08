-- GB28181录像记录表
-- 用于存储设备录像文件信息

CREATE TABLE IF NOT EXISTS records (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(20) NOT NULL,           -- 设备ID
    channel_id VARCHAR(20) NOT NULL,          -- 通道ID
    name VARCHAR(255),                        -- 录像名称
    file_path VARCHAR(512),                   -- 文件路径
    address VARCHAR(255),                     -- 存储地址
    start_time DATETIME NOT NULL,             -- 开始时间
    end_time DATETIME NOT NULL,               -- 结束时间
    secrecy INTEGER DEFAULT 0,                -- 保密属性(0-不保密,1-保密)
    type VARCHAR(20),                         -- 录像类型(time/alarm/manual/all)
    recorder_id VARCHAR(20),                  -- 录像机ID
    file_size BIGINT,                         -- 文件大小(字节)
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引以提高查询性能
CREATE INDEX IF NOT EXISTS idx_device_id ON records(device_id);
CREATE INDEX IF NOT EXISTS idx_channel_id ON records(channel_id);
CREATE INDEX IF NOT EXISTS idx_time_range ON records(start_time, end_time);
CREATE INDEX IF NOT EXISTS idx_type ON records(type);
