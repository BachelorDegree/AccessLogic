#pragma once
#include "Proto/accesslogic.pb.h"
class AccessLogicServiceImpl
{
public:
    static AccessLogicServiceImpl *GetInstance();
    static void SetInstance(AccessLogicServiceImpl *);
    static int BeforeServerStart(const char * czConf) {
        return 0;
    }
    int BeforeWorkerStart() {
        return 0;
    }
    int Hello(const ::accesslogic::HelloReq & oReq, ::accesslogic::HelloResp & oResp);
};
