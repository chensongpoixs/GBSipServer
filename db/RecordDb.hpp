/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像数据库访问层
 ******************************************************************************/

#ifndef GBSIPSERVER_DB_RECORDDB_HPP
#define GBSIPSERVER_DB_RECORDDB_HPP

#include "dto/RecordDto.hpp"
#include "oatpp-sqlite/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

namespace gbsip_server {

/**
 * 录像数据库客户端
 */
class RecordDb : public oatpp::orm::DbClient {
public:
    RecordDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
        : oatpp::orm::DbClient(executor)
    {
        // 创建表
        oatpp::String tableScript = 
            "CREATE TABLE IF NOT EXISTS records ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "device_id VARCHAR(20) NOT NULL,"
            "channel_id VARCHAR(20) NOT NULL,"
            "name VARCHAR(255),"
            "file_path VARCHAR(512),"
            "address VARCHAR(255),"
            "start_time DATETIME NOT NULL,"
            "end_time DATETIME NOT NULL,"
            "secrecy INTEGER DEFAULT 0,"
            "type VARCHAR(20),"
            "recorder_id VARCHAR(20),"
            "file_size BIGINT,"
            "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
            "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");"
            "CREATE INDEX IF NOT EXISTS idx_device_id ON records(device_id);"
            "CREATE INDEX IF NOT EXISTS idx_channel_id ON records(channel_id);"
            "CREATE INDEX IF NOT EXISTS idx_time_range ON records(start_time, end_time);"
            "CREATE INDEX IF NOT EXISTS idx_type ON records(type);";
        
        executeQuery(tableScript);
    }

    /**
     * 创建录像记录
     */
    QUERY(createRecord,
          "INSERT INTO records "
          "(device_id, channel_id, name, file_path, address, start_time, end_time, "
          "secrecy, type, recorder_id, file_size) "
          "VALUES "
          "(:record.device_id, :record.channel_id, :record.name, :record.file_path, "
          ":record.address, :record.start_time, :record.end_time, :record.secrecy, "
          ":record.type, :record.recorder_id, :record.file_size);",
          PARAM(oatpp::Object<RecordItemDto>, record))

    /**
     * 根据设备ID查询录像
     */
    QUERY(getRecordsByDeviceId,
          "SELECT * FROM records WHERE device_id = :deviceId;",
          PARAM(oatpp::String, deviceId))

    /**
     * 根据通道ID查询录像
     */
    QUERY(getRecordsByChannelId,
          "SELECT * FROM records WHERE channel_id = :channelId;",
          PARAM(oatpp::String, channelId))

    /**
     * 按时间范围查询录像
     */
    QUERY(getRecordsByTimeRange,
          "SELECT * FROM records "
          "WHERE device_id = :deviceId "
          "AND channel_id = :channelId "
          "AND start_time >= :startTime "
          "AND end_time <= :endTime "
          "ORDER BY start_time ASC;",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::String, channelId),
          PARAM(oatpp::String, startTime),
          PARAM(oatpp::String, endTime))

    /**
     * 按时间范围和类型查询录像
     */
    QUERY(getRecordsByTimeRangeAndType,
          "SELECT * FROM records "
          "WHERE device_id = :deviceId "
          "AND channel_id = :channelId "
          "AND start_time >= :startTime "
          "AND end_time <= :endTime "
          "AND type = :type "
          "ORDER BY start_time ASC;",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::String, channelId),
          PARAM(oatpp::String, startTime),
          PARAM(oatpp::String, endTime),
          PARAM(oatpp::String, type))

    /**
     * 更新录像状态
     */
    QUERY(updateRecord,
          "UPDATE records SET "
          "name = :record.name, "
          "file_path = :record.file_path, "
          "address = :record.address, "
          "file_size = :record.file_size, "
          "updated_at = CURRENT_TIMESTAMP "
          "WHERE id = :record.id;",
          PARAM(oatpp::Object<RecordItemDto>, record))

    /**
     * 删除录像记录
     */
    QUERY(deleteRecord,
          "DELETE FROM records WHERE id = :id;",
          PARAM(oatpp::Int32, id))

    /**
     * 删除设备的所有录像
     */
    QUERY(deleteRecordsByDeviceId,
          "DELETE FROM records WHERE device_id = :deviceId;",
          PARAM(oatpp::String, deviceId))

    /**
     * 获取录像总数
     */
    QUERY(getRecordCount,
          "SELECT COUNT(*) as count FROM records "
          "WHERE device_id = :deviceId "
          "AND channel_id = :channelId "
          "AND start_time >= :startTime "
          "AND end_time <= :endTime;",
          PARAM(oatpp::String, deviceId),
          PARAM(oatpp::String, channelId),
          PARAM(oatpp::String, startTime),
          PARAM(oatpp::String, endTime))
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(DbClient)

#endif // GBSIPSERVER_DB_RECORDDB_HPP
