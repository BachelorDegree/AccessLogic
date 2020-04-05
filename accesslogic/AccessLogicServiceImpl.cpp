#include "AccessLogicServiceImpl.hpp"
int AccessLogicServiceImpl::Hello(const ::accesslogic::HelloReq & oReq, ::accesslogic::HelloResp & oResp) {
	oResp.set_value("Hello");
    return 0;
}
