#include "HelloHandler.hpp"
#include "AccessLogicServiceImpl.hpp"
void HelloHandler::SetInterfaceName(void)
{
    interfaceName = "AccessLogicService.Hello";
}

void HelloHandler::Proceed(void)
{
    switch (status)
    {
    case Status::CREATE:
        this->SetStatusProcess();
        service->RequestHello(&ctx, &request, &responder, cq, cq, this);
        break;
    case Status::PROCESS:
    {
        // Firstly, spawn a new handler for next incoming RPC call
        new HelloHandler(service, cq);
        this->BeforeProcess();
        // Implement your logic here
        int iRet = AccessLogicServiceImpl::GetInstance()->Hello(request, response);
        this->SetReturnCode(iRet);
        this->SetStatusFinish();
        responder.Finish(response, grpc::Status::OK, this);
        break;
    }
    case Status::FINISH:
        delete this;
        break;
    default:
        // throw exception
        ;
    }
}

