#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <coredeps/SatelliteClient.hpp>
#include <coredeps/ContextHelper.hpp>
#include "AccessLogicServiceClient.hpp"
AccessLogicServiceClient::AccessLogicServiceClient()
{
  m_pChannel = GetChannel();
}
std::shared_ptr<grpc::Channel> AccessLogicServiceClient::GetChannel()
{
  const std::string strServiceName = "AccessLogicService";
  std::string strServer = SatelliteClient::GetInstance().GetNode(strServiceName);
  return grpc::CreateChannel(strServer, grpc::InsecureChannelCredentials());
}
int AccessLogicServiceClient::Hello(const ::accesslogic::HelloReq & oReq, ::accesslogic::HelloResp & oResp)
{
  ::accesslogic::AccessLogicService::Stub oStub{m_pChannel};
  grpc::ClientContext oContext;
  auto oStatus = oStub.Hello(&oContext, oReq, &oResp);
  if (oStatus.ok() == false)
  {
    return -1;
  }
  return ClientContextHelper(oContext).GetReturnCode();
}
