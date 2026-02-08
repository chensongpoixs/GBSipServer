/*
*  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
*
*  Please visit https://chensongpoixs.github.io for detail
*
*  Use of this source code is governed by a BSD-style license
*  that can be found in the LICENSE file in the root of the source
*  tree. An additional intellectual property rights grant can be found
*  in the file PATENTS.  All contributing project authors may
*  be found in the AUTHORS file in the root of the source tree.
*/
/*****************************************************************************
				  Author: chensong
				  date:  2025-09-17



******************************************************************************/

#ifndef _C_DEVICE_SERVICE_HPP
#define _C_DEVICE_SERVICE_HPP

#include "db/UserDb.hpp"
#include "db/DeviceDb.hpp"
#include "dto/PageDto.hpp"

#include "dto/StatusDto.hpp"

#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"

class DeviceService {
private:
	typedef oatpp::web::protocol::http::Status Status;
private:
	OATPP_COMPONENT(std::shared_ptr<DeviceDb>, m_database); // Inject database component
public:

	oatpp::Object<DeviceDto> createDevice(const oatpp::Object<DeviceDto>& dto);
	oatpp::Object<DeviceDto> updateDevice(const oatpp::Object<DeviceDto>& dto);
	oatpp::Object<DeviceDto> getDeviceById(const oatpp::Int32& id, const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection = nullptr);
	oatpp::Object<PageDto<oatpp::Object<DeviceDto>>> getAllDevices(const oatpp::UInt32& offset, const oatpp::UInt32& limit);
	oatpp::Object<StatusDto> deleteDeviceById(const oatpp::Int32& id);

};

#endif //_C_DEVICE_SERVICE_HPP
