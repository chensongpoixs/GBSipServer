-- GB28181报警记录表
-- 用于存储设备报警信息

CREATE TABLE IF NOT EXISTS alarms (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    device_id VARCHAR(20) NOT NULL,           -- 设备ID
    channel_id VARCHAR(20) NOT NULL,          -- 通道ID
    alarm_priority INTEGER NOT NULL,          -- 报警级别(1-4,1最高)
    alarm_method INTEGER NOT NULL,            -- 报警方式(1-设备,2-平台,3-其他)
    alarm_time DATETIME NOT NULL,             -- 报警时间
    alarm_description VARCHAR(512),           -- 报警描述
    longitude DOUBLE,                         -- 经度
    latitude DOUBLE,                          -- 纬度
    status INTEGER DEFAULT 0,                 -- 状态(0-未处理,1-已确认,2-已复位)
    confirmed_at DATETIME,                    -- 确认时间
    reset_at DATETIME,                        -- 复位时间
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 创建索引以提高查询性能
CREATE INDEX IF NOT EXISTS idx_alarm_device_id ON alarms(device_id);
CREATE INDEX IF NOT EXISTS idx_alarm_channel_id ON alarms(channel_id);
CREATE INDEX IF NOT EXISTS idx_alarm_time ON alarms(alarm_time);
CREATE INDEX IF NOT EXISTS idx_alarm_status ON alarms(status);
CREATE INDEX IF NOT EXISTS idx_alarm_priority ON alarms(alarm_priority);
