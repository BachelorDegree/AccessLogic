#include <colib/co_routine_specific.h>
#include "AccessLogicServiceImpl.hpp"
struct __AccessLogicServiceImplWrapper{
    AccessLogicServiceImpl * pImpl;
};
CO_ROUTINE_SPECIFIC(__AccessLogicServiceImplWrapper, g_coAccessLogicServiceImplWrapper)
AccessLogicServiceImpl *AccessLogicServiceImpl::GetInstance()
{
    return g_coAccessLogicServiceImplWrapper->pImpl;
}
void AccessLogicServiceImpl::SetInstance(AccessLogicServiceImpl *pImpl)
{
    g_coAccessLogicServiceImplWrapper->pImpl = pImpl;
}
