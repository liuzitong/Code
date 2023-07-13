#ifndef RPCSERVERSVC_HXX
#define RPCSERVERSVC_HXX

#include <QString>

//namespace CE {

class JRpcServerSvc
{
public:
    JRpcServerSvc();
    static bool                     start(const QString &svr_name);
    static void                     stop();
};

//}
#endif // RPCSERVERSVC_HXX
