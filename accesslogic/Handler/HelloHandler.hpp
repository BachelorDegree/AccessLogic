#pragma once

#include "AsyncRpcHandler.hpp"
#include "Proto/accesslogic.grpc.pb.h"

class HelloHandler final : public AsyncRpcHandler
{
public:
    HelloHandler(::accesslogic::AccessLogicService::AsyncService *service, grpc::ServerCompletionQueue *cq):
        AsyncRpcHandler(cq), service(service), responder(&ctx)
    {
        this->Proceed();
    }
    void Proceed(void) override;
    void SetInterfaceName(void) override;

private:
    ::accesslogic::AccessLogicService::AsyncService*                     service;
    ::accesslogic::HelloReq                                    request;
    ::accesslogic::HelloResp                                   response;
    grpc::ServerAsyncResponseWriter<::accesslogic::HelloResp>  responder;
};
