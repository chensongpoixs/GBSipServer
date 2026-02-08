/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像服务层
 ******************************************************************************/

#ifndef GBSIPSERVER_SERVICE_RECORDSERVICE_HPP
#define GBSIPSERVER_SERVICE_RECORDSERVICE_HPP

#include "dto/RecordDto.hpp"
#include "db/RecordDb.hpp"
#include "db/DeviceDb.hpp"
#include "sip/RecordHandler.hpp"
#include "oatpp/core/macro/component.hpp"
#include <memory>

namespace gbsip_server {

/**
 * 录像服务
 * 负责录像管理的业务逻辑
 */
class RecordService {
public:
    /**
     * 构造函数
     */
    RecordService(std::shared_ptr<RecordHandler> recordHandler);

    /**
     * 析构函数
     */
    ~RecordService();

    /**
     * 查询录像列表
     */
    oatpp::Object<RecordListDto> queryRecords(
        const oatpp::Object<RecordQueryDto>& dto);

    /**
     * 开始回放
     */
    oatpp::Object<RecordResponseDto> startPlayback(
        const oatpp::Object<PlaybackStartDto>& dto);

    /**
     * 暂停回放
     */
    oatpp::Object<RecordResponseDto> pausePlayback(
        const oatpp::Object<PlaybackControlDto>& dto);

    /**
     * 继续回放
     */
    oatpp::Object<RecordResponseDto> resumePlayback(
        const oatpp::Object<PlaybackControlDto>& dto);

    /**
     * 停止回放
     */
    oatpp::Object<RecordResponseDto> stopPlayback(
        const oatpp::Object<PlaybackControlDto>& dto);

    /**
     * 跳转回放
     */
    oatpp::Object<RecordResponseDto> seekPlayback(
        const oatpp::Object<PlaybackSeekDto>& dto);

    /**
     * 设置回放速度
     */
    oatpp::Object<RecordResponseDto> setPlaybackSpeed(
        const oatpp::Object<PlaybackSpeedDto>& dto);

    /**
     * 开始下载
     */
    oatpp::Object<RecordResponseDto> startDownload(
        const oatpp::Object<DownloadStartDto>& dto);

    /**
     * 停止下载
     */
    oatpp::Object<RecordResponseDto> stopDownload(
        const oatpp::Object<PlaybackControlDto>& dto);

private:
    /**
     * 创建错误响应
     */
    oatpp::Object<RecordListDto> createErrorResponse(
        const std::string& message, int code = 500);

    /**
     * 创建成功响应
     */
    oatpp::Object<RecordResponseDto> createSuccessResponse(
        const std::string& message, 
        const std::string& callId = "",
        const std::string& sdp = "");

    /**
     * 创建失败响应
     */
    oatpp::Object<RecordResponseDto> createFailureResponse(
        const std::string& message, int code = 500);

    /**
     * 保存录像到数据库
     */
    void saveRecordsToDb(const std::vector<oatpp::Object<RecordItemDto>>& records);

private:
    OATPP_COMPONENT(std::shared_ptr<RecordDb>, record_db_);
    OATPP_COMPONENT(std::shared_ptr<DeviceDb>, device_db_);
    std::shared_ptr<RecordHandler> record_handler_;
};

} // namespace gbsip_server

#endif // GBSIPSERVER_SERVICE_RECORDSERVICE_HPP
