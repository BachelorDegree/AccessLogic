#include "dylib_export.h"
#include "Proto/accesslogic.grpc.pb.h"
#include "AccessLogicServiceImpl.hpp"
#include "Handler/HelloHandler.hpp"

::accesslogic::AccessLogicService::AsyncService service;

const char * EXPORT_Description(void)
{
    return "accesslogic";
}

void EXPORT_DylibInit(const char * conf_file)
{
    AccessLogicServiceImpl::BeforeServerStart(conf_file);
}

grpc::Service * EXPORT_GetGrpcServiceInstance(void)
{
    return &service;
}
void EXPORT_OnCoroutineWorkerStart(void)
{
    AccessLogicServiceImpl::SetInstance(new AccessLogicServiceImpl);
    AccessLogicServiceImpl::GetInstance()->BeforeWorkerStart();
} 
void EXPORT_OnWorkerThreadStart(grpc::ServerCompletionQueue *cq)
{
  // Bind handlers

    new HelloHandler(&service, cq);
}
