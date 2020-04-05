#pragma once
#include <memory>
#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include "accesslogic.grpc.pb.h"
#include "accesslogic.pb.h"
class AccessLogicServiceClient
{
private:
  std::shared_ptr<grpc::Channel> m_pChannel;
public:
  AccessLogicServiceClient();
  static std::shared_ptr<grpc::Channel> GetChannel();
    int Hello(const ::accesslogic::HelloReq & oReq, ::accesslogic::HelloResp & oResp);
};
