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
				  date:  2025-10-12



******************************************************************************/
#include "service/DeviceService.hpp" 


oatpp::Object<DeviceDto> DeviceService::createDevice(const oatpp::Object<DeviceDto>& dto) {

	auto dbResult = m_database->createDevice(dto);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

	auto dievceId = oatpp::sqlite::Utils::getLastInsertRowId(dbResult->getConnection());

	return getDeviceById(dievceId);

}

oatpp::Object<DeviceDto> DeviceService::updateDevice(const oatpp::Object<DeviceDto>& dto) {

	auto dbResult = m_database->updateDevice(dto);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
	return getDeviceById(dto->id);

}

oatpp::Object<DeviceDto> DeviceService::getDeviceById(const oatpp::Int32& id, const oatpp::provider::ResourceHandle<oatpp::orm::Connection>& connection) {

	auto dbResult = m_database->getDeviceById(id, connection);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
	OATPP_ASSERT_HTTP(dbResult->hasMoreToFetch(), Status::CODE_404, "Device not found");

	auto result = dbResult->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();
	OATPP_ASSERT_HTTP(result->size() == 1, Status::CODE_500, "Unknown error");

	return result[0];

}

oatpp::Object<PageDto<oatpp::Object<DeviceDto>>> DeviceService::getAllDevices(const oatpp::UInt32& offset, const oatpp::UInt32& limit) {

	oatpp::UInt32 countToFetch = limit;

	if (limit > 10) {
		countToFetch = 10;
	}

	auto dbResult = m_database->getAllDevices(offset, countToFetch);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());

	auto items = dbResult->fetch<oatpp::Vector<oatpp::Object<DeviceDto>>>();

	auto page = PageDto<oatpp::Object<DeviceDto>>::createShared();
	page->offset = offset;
	page->limit = countToFetch;
	page->count = items->size();
	page->items = items;

	return page;

}

oatpp::Object<StatusDto> DeviceService::deleteDeviceById(const oatpp::Int32& id) {
	auto dbResult = m_database->deleteDeviceById(id);
	OATPP_ASSERT_HTTP(dbResult->isSuccess(), Status::CODE_500, dbResult->getErrorMessage());
	auto status = StatusDto::createShared();
	status->status = "OK";
	status->code = 200;
	status->message = "Device was successfully deleted";
	return status;
}