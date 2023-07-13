#ifndef JRPCPLAT_PKGERROR_DEF_HXX
#define JRPCPLAT_PKGERROR_DEF_HXX

namespace JRpcPlat {

enum  PkgError {
    PkgError_OK = 0,
    PkgError_ParseError     = -32700,
    PkgError_InvalidRequest = -32600,
    PkgError_MethodNotFound = -32601,
    PkgError_InvalidParams  = -32602,
    PkgError_InternalError  = -32603,
    PkgError_InvalidResponse = -32000,  // in JsonRpc2, -32000 to -32099 is custom error
    PkgError_NeedAccToken    = -32001,  // need acc token, means an error or last access token is not valid
    PkgError_NeedCtlToken    = -32002,
    PkgError_InvalidCliProfile = -32003,
    PkgError_InvalidJRpcSpec = -32004,
    PkgError_NoClientAnswere = -32005,  // post to all client, but no client answere in time
    PkgError_NoRespFromCli   = -32006,  // a client accepted request, but no response in time
    PkgError_LostSvcClient   = -32007   // the client that accepted request, but disconnected
};


}

#endif 
