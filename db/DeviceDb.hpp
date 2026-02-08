



#ifndef _C_DEVICE_DB_H_
#define _C_DEVICE_DB_H_

#include "dto/UserDto.hpp"
#include "oatpp-sqlite/orm.hpp"
#include "dto/DeviceDto.hpp"
#include "oatpp/base/Log.hpp"
#include "rtc_base/logging.h"
#include OATPP_CODEGEN_BEGIN(DbClient) //<- Begin Codegen

/**
 * DeviceDb client definitions.
 */
class DeviceDb : public oatpp::orm::DbClient {
public:

  DeviceDb(const std::shared_ptr<oatpp::orm::Executor>& executor)
    : oatpp::orm::DbClient(executor)
  {

    oatpp::orm::SchemaMigration migration(executor);
    migration.addFile(2 /* start from version 1 */, DATABASE_MIGRATIONS "/002_init_device.sql");
	//migration.addFile(2 /* start from version 1 */, DATABASE_MIGRATIONS "/001_init_device.sql");
    // TODO - Add more migrations here.
    migration.migrate(); // <-- run migrations. This guy will throw on error.

    auto version = executor->getSchemaVersion();
   // OATPP_LOGd("DeviceDb", "Migration - OK. Version={}.", version);
	RTC_LOG(LS_INFO) << "DeviceDB  Migration load version:" << version;
  }
  QUERY(createDevice,
      "INSERT INTO t_device"
      "(deviceid, type, devicename, channelcount, password, catalogsubscribe, online, siptransport, "
      "mediatransport, mediatransportmode, lastregisterat, lastkeepaliveat, updatedat, createdat, remoteip, remoteport, role) VALUES "
      "(:device.deviceid, :device.type, :device.devicename, :device.channelcount , "
      ":device.password, :device.catalogsubscribe, :device.online, :device.siptransport, "
      ":device.mediatransport, :device.mediatransportmode, :device.lastregisterat, "
      ":device.lastkeepaliveat,  :device.updatedat, :device.createdat, :device.remoteip, :device.remoteport, :device.role);",
      PARAM(oatpp::Object<DeviceDto>, device))


  //QUERY(createDevice,
  //      "INSERT INTO t_device"
  //      "(deviceid, devicename, type, channelcount, password, catalogsubscribe, online, role) VALUES "
  //      "(:device.deviceid, :device.devicename, :device.type, "
  ///  ":device.channelcount, :device.password, :device.catalogsubscribe,  :device.online, :device.role);",
  //      PARAM(oatpp::Object<DeviceDto>, device)) 
  //
  QUERY(updateDevice,
        "UPDATE t_device "
        "SET "
        " online=:device.online, "
        "WHERE "
        " id=:device.id;",
        PARAM(oatpp::Object<DeviceDto>, device))

  QUERY(getDeviceById,
        "SELECT * FROM t_device WHERE id=:id;",
        PARAM(oatpp::Int32, id))

  QUERY(getDeviceByDeviceId,
		  "SELECT * FROM t_device WHERE deviceid=:deviceId;",
		  PARAM(oatpp::String, deviceId))
  QUERY(getAllDevices,
        "SELECT * FROM t_device LIMIT :limit OFFSET :offset;",
        PARAM(oatpp::UInt32, offset),
        PARAM(oatpp::UInt32, limit))

  QUERY(deleteDeviceById,
        "DELETE FROM t_device WHERE id=:id;",
        PARAM(oatpp::Int32, id))

};

#include OATPP_CODEGEN_END(DbClient) //<- End Codegen

#endif //CRUD_USERDB_HPP
