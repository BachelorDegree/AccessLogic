#include <iostream>
#include <thread>
#include<vector>
#include <atomic>
#include "AccessLogicServiceClient/AccessLogicServiceClient.hpp"
#include <coredeps/SatelliteClient.hpp>
#include <unistd.h>
int main()
{
    	SatelliteClient::GetInstance().SetServer("10.0.0.102:5553");
  	AccessLogicServiceClient alClient;
	accesslogic::HelloReq alReq;
	accesslogic::HelloResp alRsp;
	alReq.set_key("test");
	int iRet = alClient.Hello(alReq, alRsp);
	if(iRet != 0){
		std::cout<<iRet<<std::endl;
	}
	else
	{
		std::cout<<alRsp.value()<<std::endl;
	}

	return 0;
}
