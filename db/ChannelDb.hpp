/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2026-02-09
				  
				  通道数据库访问类
******************************************************************************/

#ifndef _C_CHANNEL_DB_HPP
#define _C_CHANNEL_DB_HPP

#include "dto/ChannelDto.hpp"
#include "dto/PageDto.hpp"
#include "oatpp-sqlite/orm.hpp"

#include OATPP_CODEGEN_BEGIN(DbClient)

/**
 * 通道数据库客户端
 */
class ChannelDb : public oatpp::orm::DbClient {
public:
  
  ChannelDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {
    // 创建表
    oatpp::String tableScript = 
      "CREATE TABLE IF NOT EXISTS t_channel ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "channel_id VARCHAR(20) UNIQUE NOT NULL,"
      "device_id VARCHAR(20) NOT NULL,"
      "channel_name VARCHAR(255),"
      "manufacturer VARCHAR(100),"
      "model VARCHAR(100),"
      "owner VARCHAR(100),"
      "civil_code VARCHAR(6),"
      "address VARCHAR(255),"
      "parental INTEGER DEFAULT 0,"
      "parent_id VARCHAR(20),"
      "safety_way INTEGER DEFAULT 0,"
      "register_way INTEGER DEFAULT 1,"
      "secrecy INTEGER DEFAULT 0,"
      "status VARCHAR(10) DEFAULT 'OFF',"
      "longitude REAL DEFAULT 0,"
      "latitude REAL DEFAULT 0,"
      "ptz_type INTEGER DEFAULT 0,"
      "create_time INTEGER,"
      "update_time INTEGER"
      ");";
    
    executeQuery(tableScript);
    
    // 创建索引
    executeQuery("CREATE INDEX IF NOT EXISTS idx_channel_device_id ON t_channel(device_id);");
    executeQuery("CREATE INDEX IF NOT EXISTS idx_channel_status ON t_channel(status);");
  }
  
  QUERY(createChannel,
        "INSERT INTO t_channel "
        "(channel_id, device_id, channel_name, manufacturer, model, owner, "
        "civil_code, address, parental, parent_id, safety_way, register_way, "
        "secrecy, status, longitude, latitude, ptz_type, create_time, update_time) "
        "VALUES "
        "(:channel.channelId, :channel.deviceId, :channel.name, :channel.manufacturer, "
        ":channel.model, :channel.owner, :channel.civilCode, :channel.address, "
        ":channel.parental, :channel.parentId, :channel.safetyWay, :channel.registerWay, "
        ":channel.secrecy, :channel.status, :channel.longitude, :channel.latitude, "
        ":channel.ptzType, :channel.createTime, :channel.updateTime);",
        PARAM(oatpp::Object<ChannelDto>, channel))
  
  QUERY(updateChannel,
        "UPDATE t_channel "
        "SET channel_name=:channel.name, manufacturer=:channel.manufacturer, "
        "model=:channel.model, owner=:channel.owner, civil_code=:channel.civilCode, "
        "address=:channel.address, parental=:channel.parental, parent_id=:channel.parentId, "
        "safety_way=:channel.safetyWay, register_way=:channel.registerWay, "
        "secrecy=:channel.secrecy, status=:channel.status, longitude=:channel.longitude, "
        "latitude=:channel.latitude, ptz_type=:channel.ptzType, update_time=:channel.updateTime "
        "WHERE channel_id=:channel.channelId;",
        PARAM(oatpp::Object<ChannelDto>, channel))
  
  QUERY(getChannelById,
        "SELECT * FROM t_channel WHERE channel_id=:channelId;",
        PARAM(oatpp::String, channelId))
  
  QUERY(getChannelsByDeviceId,
        "SELECT * FROM t_channel WHERE device_id=:deviceId ORDER BY channel_id;",
        PARAM(oatpp::String, deviceId))
  
  QUERY(getAllChannels,
        "SELECT * FROM t_channel LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::UInt32, offset),
        PARAM(oatpp::UInt32, limit))
  
  QUERY(getChannelCount,
        "SELECT COUNT(*) as count FROM t_channel;")
  
  QUERY(getChannelCountByDeviceId,
        "SELECT COUNT(*) as count FROM t_channel WHERE device_id=:deviceId;",
        PARAM(oatpp::String, deviceId))
  
  QUERY(deleteChannelById,
        "DELETE FROM t_channel WHERE channel_id=:channelId;",
        PARAM(oatpp::String, channelId))
  
  QUERY(deleteChannelsByDeviceId,
        "DELETE FROM t_channel WHERE device_id=:deviceId;",
        PARAM(oatpp::String, deviceId))
  
  QUERY(updateChannelStatus,
        "UPDATE t_channel SET status=:status, update_time=:updateTime "
        "WHERE channel_id=:channelId;",
        PARAM(oatpp::String, channelId),
        PARAM(oatpp::String, status),
        PARAM(oatpp::Int64, updateTime))
  
};

#include OATPP_CODEGEN_END(DbClient)

#endif /* _C_CHANNEL_DB_HPP */
