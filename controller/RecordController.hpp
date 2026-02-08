/******************************************************************************
 *  Copyright (c) 2025 The CRTC project authors . All Rights Reserved.
 *
 *  Author: chensong
 *  Date: 2026-02-09
 *  Description: 录像REST控制器
 ******************************************************************************/

#ifndef GBSIPSERVER_CONTROLLER_RECORDCONTROLLER_HPP
#define GBSIPSERVER_CONTROLLER_RECORDCONTROLLER_HPP

#include "service/RecordService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include OATPP_CODEGEN_BEGIN(ApiController)

namespace gbsip_server {

/**
 * 录像REST控制器
 * 提供录像管理的HTTP API接口
 */
class RecordController : public oatpp::web::server::api::ApiController {
public:
    RecordController(const std::shared_ptr<ObjectMapper>& objectMapper)
        : oatpp::web::server::api::ApiController(objectMapper)
    {
    }

    static std::shared_ptr<RecordController> createShared(
        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
    {
        return std::make_shared<RecordController>(objectMapper);
    }

    /**
     * 设置RecordService
     */
    void setRecordService(std::shared_ptr<RecordService> service) {
        record_service_ = service;
    }

    /**
     * 查询录像列表
     */
    ENDPOINT_INFO(queryRecords) {
        info->summary = "Query record list";
        info->description = "Query device recording files";
        info->addConsumes<Object<RecordQueryDto>>("application/json");
        info->addResponse<Object<RecordListDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/query", queryRecords,
             BODY_DTO(Object<RecordQueryDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordListDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->queryRecords(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 开始回放
     */
    ENDPOINT_INFO(startPlayback) {
        info->summary = "Start playback";
        info->description = "Start recording playback";
        info->addConsumes<Object<PlaybackStartDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/playback/start", startPlayback,
             BODY_DTO(Object<PlaybackStartDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->startPlayback(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 暂停回放
     */
    ENDPOINT_INFO(pausePlayback) {
        info->summary = "Pause playback";
        info->description = "Pause recording playback";
        info->addConsumes<Object<PlaybackControlDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/playback/pause", pausePlayback,
             BODY_DTO(Object<PlaybackControlDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->pausePlayback(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 继续回放
     */
    ENDPOINT_INFO(resumePlayback) {
        info->summary = "Resume playback";
        info->description = "Resume recording playback";
        info->addConsumes<Object<PlaybackControlDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/playback/resume", resumePlayback,
             BODY_DTO(Object<PlaybackControlDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->resumePlayback(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 停止回放
     */
    ENDPOINT_INFO(stopPlayback) {
        info->summary = "Stop playback";
        info->description = "Stop recording playback";
        info->addConsumes<Object<PlaybackControlDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/playback/stop", stopPlayback,
             BODY_DTO(Object<PlaybackControlDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->stopPlayback(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 跳转回放
     */
    ENDPOINT_INFO(seekPlayback) {
        info->summary = "Seek playback";
        info->description = "Seek to specific time in playback";
        info->addConsumes<Object<PlaybackSeekDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/playback/seek", seekPlayback,
             BODY_DTO(Object<PlaybackSeekDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->seekPlayback(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 调整回放速度
     */
    ENDPOINT_INFO(setPlaybackSpeed) {
        info->summary = "Set playback speed";
        info->description = "Adjust playback speed (0.25x, 0.5x, 1x, 2x, 4x)";
        info->addConsumes<Object<PlaybackSpeedDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/playback/speed", setPlaybackSpeed,
             BODY_DTO(Object<PlaybackSpeedDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->setPlaybackSpeed(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 开始下载
     */
    ENDPOINT_INFO(startDownload) {
        info->summary = "Start download";
        info->description = "Start recording download";
        info->addConsumes<Object<DownloadStartDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/download/start", startDownload,
             BODY_DTO(Object<DownloadStartDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->startDownload(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

    /**
     * 停止下载
     */
    ENDPOINT_INFO(stopDownload) {
        info->summary = "Stop download";
        info->description = "Stop recording download";
        info->addConsumes<Object<PlaybackControlDto>>("application/json");
        info->addResponse<Object<RecordResponseDto>>(Status::CODE_200, "application/json");
        info->addTag("Record Management");
    }
    ENDPOINT("POST", "/api/v1/record/download/stop", stopDownload,
             BODY_DTO(Object<PlaybackControlDto>, dto))
    {
        if (!record_service_) {
            auto response = RecordResponseDto::createShared();
            response->code = 500;
            response->status = "ERROR";
            response->message = "RecordService not initialized";
            return createDtoResponse(Status::CODE_500, response);
        }
        
        auto result = record_service_->stopDownload(dto);
        return createDtoResponse(Status::CODE_200, result);
    }

private:
    std::shared_ptr<RecordService> record_service_;
};

} // namespace gbsip_server

#include OATPP_CODEGEN_END(ApiController)

#endif // GBSIPSERVER_CONTROLLER_RECORDCONTROLLER_HPP
