/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 报警数据库访问层
 ******************************************************************************/

#ifndef GBSIPSERVER_DB_ALARMDB_HPP
#define GBSIPSERVER_DB_ALARMDB_HPP

#include "dto/AlarmDto.hpp"
#include "oatpp-sqlite/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

namespace gbsip_server {

/**
 * 报警数据库客户端
 */
class AlarmDb : public oatpp::orm::DbClient {
public:
    AlarmDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor)
    {
        // 创建表
        oatpp::String tableScript = 
            "CREATE TABLE IF NOT EXISTS alarms ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "device_id VARCHAR(20) NOT NULL,"
            "channel_id VARCHAR(20) NOT NULL,"
            "alarm_priority INTEGER NOT NULL,"
            "alarm_method INTEGER NOT NULL,"
            "alarm_time DATETIME NOT NULL,"
            "alarm_description VARCHAR(512),"
            "longitude DOUBLE,"
            "latitude DOUBLE,"
            "status INTEGER DEFAULT 0,"
            "confirmed_at DATETIME,"
            "reset_at DATETIME,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");"
            "CREATE INDEX IF NOT EXISTS idx_alarm_device_id ON alarms(device_id);"
            "CREATE INDEX IF NOT EXISTS idx_alarm_channel_id ON alarms(channel_id);"
            "CREATE INDEX IF NOT EXISTS idx_alarm_time ON alarms(alarm_time);"
            "CREATE INDEX IF NOT EXISTS idx_alarm_status ON alarms(status);"
            "CREATE INDEX IF NOT EXISTS idx_alarm_priority ON alarms(alarm_priority);";
        
        executeQuery(tableScript);
    }

    /**
     * 创建报警记录
     */
    QUERY(createAlarm,
          "INSERT INTO alarms "
          "(device_id, channel_id, alarm_priority, alarm_method, alarm_time, "
          "alarm_description, longitude, latitude, status) "
          "VALUES "
          "(:alarm.device_id, :alarm.channel_id, :alarm.alarm_priority, "
          ":alarm.alarm_method, :alarm.alarm_time, :alarm.alarm_description, "
          ":alarm.longitude, :alarm.latitude, :alarm.status);",
          PARAM(oatpp::Object<AlarmItemDto>, alarm))

    /**
     * 根据设备ID查询报警
     */
    QUERY(getAlarmsByDeviceId,
          "SELECT * FROM alarms WHERE device_id = :deviceId "
          "ORDER BY alarm_time DESC;",
          PARAM(oatpp::String, deviceId))

    /**
     * 根据通道ID查询报警
     */
    QUERY(getAlarmsByChannelId,
          "SELECT * FROM alarms WHERE channel_id = :channelId "
          "ORDER BY alarm_time DESC;",
          PARAM(oatpp::String, channelId))

    /**
     * 按时间范围查询报警
     */
    QUERY(getAlarmsByTimeRange,
          "SELECT * FROM alarms "
          "WHERE device_id = :deviceId "
          "AND alarm_time >= :startTime "
          "AND alarm_time <= :endTime "
          "ORDER BY alarm_time DESC;",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::String, startTime),
          PARAM(oatpp::String, endTime))

    /**
     * 按时间范围和优先级查询报警
     */
    QUERY(getAlarmsByTimeRangeAndPriority,
          "SELECT * FROM alarms "
          "WHERE device_id = :deviceId "
          "AND alarm_time >= :startTime "
          "AND alarm_time <= :endTime "
          "AND alarm_priority >= :startPriority "
          "AND alarm_priority <= :endPriority "
          "ORDER BY alarm_time DESC;",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::String, startTime),
          PARAM(oatpp::String, endTime),
          PARAM(oatpp::Int32, startPriority),
          PARAM(oatpp::Int32, endPriority))

    /**
     * 分页查询报警
     */
    QUERY(getAlarmsPaged,
          "SELECT * FROM alarms "
          "WHERE device_id = :deviceId "
          "ORDER BY alarm_time DESC "
          "LIMIT :limit OFFSET :offset;",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::Int32, limit),
          PARAM(oatpp::Int32, offset))

    /**
     * 根据ID获取报警
     */
    QUERY(getAlarmById,
          "SELECT * FROM alarms WHERE id = :id;",
          PARAM(oatpp::Int32, id))

    /**
     * 更新报警状态
     */
    QUERY(updateAlarmStatus,
          "UPDATE alarms SET "
          "status = :status, "
          "updated_at = CURRENT_TIMESTAMP "
          "WHERE id = :id;",
          PARAM(oatpp::Int32, id),
          PARAM(oatpp::Int32, status))

    /**
     * 确认报警
     */
    QUERY(confirmAlarm,
          "UPDATE alarms SET "
          "status = 1, "
          "confirmed_at = CURRENT_TIMESTAMP, "
          "updated_at = CURRENT_TIMESTAMP "
          "WHERE id = :id;",
          PARAM(oatpp::Int32, id))

    /**
     * 复位报警
     */
    QUERY(resetAlarm,
          "UPDATE alarms SET "
          "status = 2, "
          "reset_at = CURRENT_TIMESTAMP, "
          "updated_at = CURRENT_TIMESTAMP "
          "WHERE id = :id;",
          PARAM(oatpp::Int32, id))

    /**
     * 删除报警记录
     */
    QUERY(deleteAlarm,
          "DELETE FROM alarms WHERE id = :id;",
          PARAM(oatpp::Int32, id))

    /**
     * 删除设备的所有报警
     */
    QUERY(deleteAlarmsByDeviceId,
          "DELETE FROM alarms WHERE device_id = :deviceId;",
          PARAM(oatpp::String, deviceId))

    /**
     * 获取报警总数
     */
    QUERY(getAlarmCount,
          "SELECT COUNT(*) as count FROM alarms "
          "WHERE device_id = :deviceId;",
          PARAM(oatpp::String, deviceId))

    /**
     * 获取未处理报警数
     */
    QUERY(getUnhandledAlarmCount,
          "SELECT COUNT(*) as count FROM alarms "
          "WHERE device_id = :deviceId AND status = 0;",
          PARAM(oatpp::String, deviceId))
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(DbClient)

#endif // GBSIPSERVER_DB_ALARMDB_HPP
