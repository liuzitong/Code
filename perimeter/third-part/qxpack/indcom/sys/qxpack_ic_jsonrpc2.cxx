// ////////////////////////////////////////////////////////////////////////////
/*!
 * @file qxpack_ic_jsonrpc2
 * @date 2019/04
 * @author nightwing
 * @verbatim
 *     <author>  < date >     < history memo >
 *     nightwing  2019/02/22   code-review, fixed some bugs
 *     nightwing  2019/04      added members..
 *     nightwing  2019/04/06   fixed IcJsonRpc2Error::make()
 *     nightwing  2019/11      fixed the qRegisterMetaType
 * @endverbatim
 */
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_JSONRPC2_CXX
#define QXPACK_IC_JSONRPC2_CXX

#include "qxpack_ic_jsonrpc2.hxx"

#include <QJsonDocument>
#include <QJsonValue>
#include <QMetaType>

#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//  IcJsonRpc2Request
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// IcJsonRpc2Request private object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivReq( o )  T_ObjCast( IcJsonRpc2RequestPriv *, o )
class  QXPACK_IC_HIDDEN IcJsonRpc2RequestPriv :
                               public IcPImplPrivTemp< IcJsonRpc2RequestPriv >
{
private:
    QJsonObject  m_params_ob;  QJsonArray  m_params_ja;
    QString      m_method, m_id_str;
    int   m_id;  bool m_is_notice;
public:
    IcJsonRpc2RequestPriv ( )
    { m_id = 0; m_is_notice = false; }

    IcJsonRpc2RequestPriv ( const IcJsonRpc2RequestPriv & other )
    {
        m_id  = other.m_id;   m_id_str = other.m_id_str;
        m_method    = other.m_method;
        m_params_ob = other.m_params_ob;
        m_params_ja = other.m_params_ja;
        m_is_notice = other.m_is_notice;
    }
    virtual ~IcJsonRpc2RequestPriv ( );

    inline int          id()       const { return m_id; }
    inline QString      idStr()    const { return m_id_str; }
    inline QString      method()   const { return m_method; }
    inline QJsonObject  paramsOb() const { return m_params_ob; }
    inline QJsonArray   paramsJa() const { return m_params_ja; }
    inline bool isWaitForResult( ) const { return ! m_is_notice; }
    inline bool isParamsJa( )      const { return ! m_params_ja.isEmpty();}
    inline bool isIdStr ( )        const { return ! m_id_str.isEmpty();   }
    inline bool isNotice( )        const { return m_is_notice; }

    inline void setId    ( int i  )          { m_id    =i; m_is_notice=false;}
    inline void setId    ( const QString &i ){ m_id_str=i; m_is_notice=false;}
    inline void setMethod( const QString &m ){ m_method=m;  }

    inline void setParams  ( const QJsonArray &jv )
    { m_params_ja = jv; m_params_ob = QJsonObject(); }

    inline void setParams( const QJsonObject&jo )
    { m_params_ob = jo; m_params_ja = QJsonArray(); }

    inline void setIsWaitForResult( bool sw ) { Q_UNUSED(sw); }

    inline void setIsNotice ( bool sw )
    { m_is_notice = sw; if ( sw ) { m_id_str = QString(); }}

    QJsonObject make ( ) const;
    bool        loadFrom ( const QJsonObject &, IcJsonRpc2Error::Code *e );
};

// ============================================================================
// DTOR
// ============================================================================
    IcJsonRpc2RequestPriv :: ~IcJsonRpc2RequestPriv( )
{
}

// ============================================================================
// generate the Json Data
// ============================================================================
QJsonObject  IcJsonRpc2RequestPriv :: make( ) const
{
    QJsonObject jo;
    jo.insert( QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    jo.insert( QStringLiteral("method"), m_method );

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "params" member can be omitted
    // ------------------------------------------------------------------------
    if        ( ! m_params_ob.isEmpty()) {
        jo.insert( QStringLiteral("params"), m_params_ob );
    } else if ( ! m_params_ja.isEmpty()) {
        jo.insert( QStringLiteral("params"), m_params_ja );
    } else { ;  } // no params for this request.

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.:  no "id" -- means a notification request
    // ------------------------------------------------------------------------
    if ( ! m_is_notice ) {
        jo.insert( // consider string id as first
           QStringLiteral("id"),
           ( isIdStr() ? QJsonValue( m_id_str ) : QJsonValue( m_id ))
        );
    }

    return jo;
}

// ============================================================================
// load data from json object
// ============================================================================
bool     IcJsonRpc2RequestPriv :: loadFrom (
    const QJsonObject &jo, IcJsonRpc2Error::Code *e
) {
    // ------------------------------------------------------------------------
    // check header
    // ------------------------------------------------------------------------
    if ( jo.isEmpty() ) {
        if ( e != Q_NULLPTR ) { *e = IcJsonRpc2Error::Code_ParseError; }
        return false;
    }
    if ( jo.value("jsonrpc").toString() != QStringLiteral("2.0")) {
        if ( e != Q_NULLPTR ) { *e = IcJsonRpc2Error::Code_InvalidRequest; }
        return false;
    }
    QJsonValue jv; bool is_chk_ok = false;

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "id" can be string, number, null value
    // in request, "id" can be string, number, no existed ( for notification )
    // ------------------------------------------------------------------------
    jv = jo.value("id"); is_chk_ok = false;
    bool is_notice = false; int id = 0; QString id_str;
    if        ( jv.isDouble() ) {
        id = jv.toInt(0);       is_chk_ok = true;
    } else if ( jv.isString() ) {
        id_str = jv.toString(); is_chk_ok = ! id_str.isEmpty();
    } else if ( jv.isNull())    {
        is_notice = true;       is_chk_ok = true;
    } else { is_notice = true; is_chk_ok = true; } // not existed.

    if ( ! is_chk_ok ) {
        if ( e != Q_NULLPTR ) { *e = IcJsonRpc2Error::Code_InvalidRequest; }
        return false;
    }

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "method" is a string value
    // ------------------------------------------------------------------------
    QString  me = jo.value("method").toString();
    if ( me.isEmpty() ) {
        if ( e != Q_NULLPTR ) { *e = IcJsonRpc2Error::Code_InvalidRequest; }
        return false;
    }

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "params" is array or object or not existed.
    // ------------------------------------------------------------------------
    jv = jo.value("params");
    QJsonObject params_ob; QJsonArray params_ja;
    if        ( jv.isObject())    {
        params_ob = jv.toObject();
    } else if ( jv.isArray() )    {
        params_ja = jv.toArray();
    } else if ( jv.isUndefined()) {
        ; // undefined means no 'params'
    } else {
        if ( e != Q_NULLPTR ) { *e = IcJsonRpc2Error::Code_InvalidRequest; }
        return false;
    }

    // ------------------------------------------------------------------------
    // store value
    // NOTE: if params_ob and params_ja are empty, means no params
    // ------------------------------------------------------------------------
    m_id = id;  m_id_str = id_str; m_is_notice = is_notice;
    m_params_ob = params_ob; m_params_ja = params_ja;
    m_method = me;

    return true;
}

// ////////////////////////////////////////////////////////////////////////////
// IcJsonRpc2Request wrap API
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_jrr_reg = false;
static void gRegJrr( )
{
    if ( ! g_is_jrr_reg ) {
        qRegisterMetaType<QxPack::IcJsonRpc2Request>();
        qRegisterMetaType<QxPack::IcJsonRpc2Request>("QxPack::IcJsonRpc2Request");
        qRegisterMetaType<QxPack::IcJsonRpc2Request>("QxPack::IcJsonRpc2Request&");
        qRegisterMetaType<QxPack::IcJsonRpc2Request>("const QxPack::IcJsonRpc2Request&");
        g_is_jrr_reg = true;
    }
}

// ============================================================================
// ctor ( create empty object )
// ============================================================================
IcJsonRpc2Request :: IcJsonRpc2Request( )
{
    gRegJrr(); m_obj = nullptr;
}

// ============================================================================
// create by json object
// ============================================================================
IcJsonRpc2Request :: IcJsonRpc2Request ( const QJsonObject &jo )
{
    gRegJrr(); m_obj = nullptr;
    IcJsonRpc2Request req;
    if ( req.loadFrom( jo )) { *this = req; }
}

// ============================================================================
// dtor
// ============================================================================
   IcJsonRpc2Request :: ~IcJsonRpc2Request( )
{
    if ( m_obj != nullptr ) {
        IcJsonRpc2RequestPriv::attach( &m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( create by spec. id, method, params )
// ============================================================================
IcJsonRpc2Request :: IcJsonRpc2Request (
   int id, const QString &me, const QJsonObject &par
) {
   gRegJrr(); m_obj = nullptr;
   if ( ! me.isEmpty() ) {
       IcJsonRpc2RequestPriv::buildIfNull( &m_obj );
       T_PrivReq( m_obj )->setId    ( id  );
       T_PrivReq( m_obj )->setMethod( me  );
       T_PrivReq( m_obj )->setParams( par );
   }
}

IcJsonRpc2Request :: IcJsonRpc2Request(
    int id, const QString &me, const QJsonArray &par
) {
    gRegJrr(); m_obj = nullptr;
    if ( ! me.isEmpty() ) {
        IcJsonRpc2RequestPriv::buildIfNull( &m_obj );
        T_PrivReq( m_obj )->setId    ( id  );
        T_PrivReq( m_obj )->setMethod( me  );
        T_PrivReq( m_obj )->setParams( par );
    }
}

// ============================================================================
// ctor ( create object by string id, method, params )
// ============================================================================
IcJsonRpc2Request :: IcJsonRpc2Request (
    const QString &id, const QString &me, const QJsonObject &par
) {
    gRegJrr(); m_obj = nullptr;
    if ( ! id.isEmpty() && ! me.isEmpty() ) {
        IcJsonRpc2RequestPriv::buildIfNull( & m_obj );
        T_PrivReq( m_obj )->setId    ( id  );
        T_PrivReq( m_obj )->setMethod( me  );
        T_PrivReq( m_obj )->setParams( par );
    }
}

IcJsonRpc2Request :: IcJsonRpc2Request (
    const QString &id, const QString &me, const QJsonArray &par
) {
    gRegJrr(); m_obj = nullptr;
    if ( ! id.isEmpty() && ! me.isEmpty() ) {
        IcJsonRpc2RequestPriv::buildIfNull( & m_obj );
        T_PrivReq( m_obj )->setId    ( id  );
        T_PrivReq( m_obj )->setMethod( me  );
        T_PrivReq( m_obj )->setParams( par );
    }
}

// ============================================================================
// ctor ( create notification by method, params )
// ============================================================================
IcJsonRpc2Request :: IcJsonRpc2Request (
    const QString &me, const QJsonObject &par
) {
    gRegJrr(); m_obj = nullptr;
    if ( ! me.isEmpty()) {
        IcJsonRpc2RequestPriv::buildIfNull( & m_obj );
        T_PrivReq( m_obj )->setMethod  ( me   );
        T_PrivReq( m_obj )->setParams  ( par  );
        T_PrivReq( m_obj )->setIsNotice( true );
    }
}

IcJsonRpc2Request :: IcJsonRpc2Request (
    const QString &me, const QJsonArray &par
) {
    gRegJrr(); m_obj = nullptr;
    if ( ! me.isEmpty() ) {
        IcJsonRpc2RequestPriv::buildIfNull( & m_obj );
        T_PrivReq( m_obj )->setMethod  ( me  );
        T_PrivReq( m_obj )->setParams  ( par );
        T_PrivReq( m_obj )->setIsNotice( true);
    }
}

// ============================================================================
// ctor ( create by another object )
// ============================================================================
IcJsonRpc2Request :: IcJsonRpc2Request( const IcJsonRpc2Request &other )
{
    gRegJrr(); m_obj = nullptr;
    IcJsonRpc2RequestPriv::attach( & m_obj, T_PtrCast( void**, & other.m_obj));
}

// ============================================================================
// ctor ( copy assign to other object )
// ============================================================================
IcJsonRpc2Request& IcJsonRpc2Request :: operator =
                                       ( const IcJsonRpc2Request &other )
{
    IcJsonRpc2RequestPriv::attach( & m_obj, T_PtrCast( void**, &other.m_obj));
    return *this;
}

// ============================================================================
// methods wrapper
// ============================================================================
bool          IcJsonRpc2Request :: isNull( ) const
{ return ( m_obj == nullptr ); }

QJsonObject   IcJsonRpc2Request :: make( ) const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->make() : QJsonObject()); }

int           IcJsonRpc2Request :: id( ) const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->id() : 0 ); }

QString       IcJsonRpc2Request :: idStr() const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->idStr() : QString()); }

QString       IcJsonRpc2Request :: method( ) const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->method() : QString()); }

QJsonObject   IcJsonRpc2Request :: params( ) const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->paramsOb() : QJsonObject());}

QJsonArray    IcJsonRpc2Request :: paramsJa() const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->paramsJa() : QJsonArray()); }

bool          IcJsonRpc2Request :: isNotification() const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->isNotice() : false ); }

bool          IcJsonRpc2Request :: isIdStr() const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->isIdStr() : false ); }

bool          IcJsonRpc2Request :: isParamsJa() const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->isParamsJa() : false ); }

bool          IcJsonRpc2Request :: isWaitForResult( ) const
{ return ( m_obj != nullptr ? T_PrivReq( m_obj )->isWaitForResult():false );}

void  IcJsonRpc2Request :: setId    ( int id )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setId( id ); }

void  IcJsonRpc2Request :: setId    ( const QString &id )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setId( id ); }

void  IcJsonRpc2Request :: setParams( const QJsonObject &p )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setParams( p ); }

void  IcJsonRpc2Request :: setParams( const QJsonArray  &p )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setParams( p ); }

void  IcJsonRpc2Request :: setMethod( const QString &m )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setMethod( m ); }

void  IcJsonRpc2Request :: setIsWaitForResult ( bool sw )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setIsWaitForResult( sw ); }

void  IcJsonRpc2Request :: setIsNotification  ( bool sw )
{ IcJsonRpc2RequestPriv::instanceCow( & m_obj )->setIsNotice( sw ); }

bool  IcJsonRpc2Request :: loadFrom (
    const QByteArray &ba, IcJsonRpc2Error::Code *e
) { return this->loadFrom( QJsonDocument::fromJson( ba ).object(), e ); }

bool  IcJsonRpc2Request :: loadFrom (
    const QJsonObject &jo, IcJsonRpc2Error::Code *e
) {
    if ( ! jo.isEmpty() ) {
        return IcJsonRpc2RequestPriv::instanceCow( & m_obj )->loadFrom( jo, e);
    } else {
        return false;
    }
}




// ////////////////////////////////////////////////////////////////////////////
//
//  IcJsonRpc2Result
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
// IcJsonRpc2ResultPriv object
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivRsl( o )  T_ObjCast( IcJsonRpc2ResultPriv *, o )
class  QXPACK_IC_HIDDEN IcJsonRpc2ResultPriv :
                 public IcPImplPrivTemp< IcJsonRpc2ResultPriv >
{
private:
    QJsonValue m_rsl; QString m_id_str; int m_id;
public:
    IcJsonRpc2ResultPriv ( ) : m_rsl( QJsonValue::Null ), m_id(0) { }

    IcJsonRpc2ResultPriv ( const IcJsonRpc2ResultPriv &other )
    {
        m_id = other.m_id; m_id_str = other.m_id_str; m_rsl = other.m_rsl;
    }
    virtual ~IcJsonRpc2ResultPriv ( );

    inline int        id     ( ) const { return m_id;     }
    inline QString    idStr  ( ) const { return m_id_str; }
    inline QJsonValue result ( ) const { return m_rsl;    }

    inline bool isIdStr ( ) const { return ! m_id_str.isEmpty(); }

    inline void setId     ( int id ) { m_id = id; m_id_str = QString(); }
    inline void setId     ( const QString    &id ) { m_id_str = id; }
    inline void setResult ( const QJsonValue &jv ) { m_rsl = jv; }

    QJsonObject  make ( ) const;
    bool         loadFrom ( const QJsonObject & );
};

// ============================================================================
// dtor
// ============================================================================
IcJsonRpc2ResultPriv :: ~IcJsonRpc2ResultPriv( )
{

}

// ============================================================================
// load from the json object
// ============================================================================
bool    IcJsonRpc2ResultPriv :: loadFrom ( const QJsonObject &jo )
{
    // ------------------------------------------------------------------------
    // check header
    // ------------------------------------------------------------------------
    if ( jo.isEmpty() ) { return false; }
    if ( jo.value("jsonrpc").toString() != QStringLiteral("2.0"))
    { return false; }
    QJsonValue jv; bool is_chk_ok = false;

    // ------------------------------------------------------------------------
    // JsonRpc2 spec.: id can be integer or string
    // ------------------------------------------------------------------------
    jv = jo.value("id"); is_chk_ok = false;
    int id = 0;  QString  id_str;
    if        ( jv.isDouble() ) {
        id = jv.toInt(0);        is_chk_ok = true;
    } else if ( jv.isString() ) {
        id_str = jv.toString();  is_chk_ok = ! id_str.isEmpty();
    } else { ; }

    if ( ! is_chk_ok ) { return false; }

    // ------------------------------------------------------------------------
    // JsonRpc2 spec.: result REQUIRED, null, string, object, array, double ...
    // ------------------------------------------------------------------------
    jv = jo.value("result");
    if ( jv.isUndefined() ) { return false; }

    // ------------------------------------------------------------------------
    // store value
    // ------------------------------------------------------------------------
    m_id  = id;  m_id_str = id_str;
    m_rsl = jv;
    return true;
}

// ============================================================================
// make the data into QJsonObject
// ============================================================================
QJsonObject  IcJsonRpc2ResultPriv :: make( ) const
{
    QJsonObject jo;
    jo.insert( QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    jo.insert( QStringLiteral("result"), m_rsl );
    jo.insert(
      QStringLiteral("id"),
      ( isIdStr() ? QJsonValue( m_id_str ) : QJsonValue( m_id ))
    );
    return jo;
}

// ////////////////////////////////////////////////////////////////////////////
// IcJsonRpc2Result API wrapper
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_jrs_reg = false;
static void gRegJrs( )
{
    if ( ! g_is_jrs_reg ) {
        qRegisterMetaType<QxPack::IcJsonRpc2Result>(); g_is_jrs_reg = true;
    }
}

// ============================================================================
// ctor ( create empty object )
// ============================================================================
IcJsonRpc2Result :: IcJsonRpc2Result ( )
{
    gRegJrs(); m_obj = nullptr;
}

// ============================================================================
// ctor ( create by integer id )
// ============================================================================
IcJsonRpc2Result :: IcJsonRpc2Result ( int id, const QJsonValue &rsl )
{
    gRegJrs(); m_obj = nullptr;
    IcJsonRpc2ResultPriv::buildIfNull( & m_obj );
    T_PrivRsl( m_obj )->setId ( id );
    T_PrivRsl( m_obj )->setResult(
        ( !rsl.isUndefined() ? rsl : QJsonValue( QJsonValue::Null ))
    );
}

IcJsonRpc2Result :: IcJsonRpc2Result ( const QString &i, const QJsonValue &r )
{
    gRegJrs(); m_obj = nullptr;
    IcJsonRpc2RequestPriv::buildIfNull( & m_obj );
    T_PrivRsl( m_obj )->setId ( i );
    T_PrivRsl( m_obj )->setResult (
        ( !r.isUndefined() ? r : QJsonValue( QJsonValue::Null ))
    );
}

// ============================================================================
// dtor
// ============================================================================
    IcJsonRpc2Result :: ~IcJsonRpc2Result( )
{
    if ( m_obj != nullptr ) {
        IcJsonRpc2ResultPriv::attach( & m_obj, nullptr );
    }
}

// ============================================================================
// ctor ( create by another object )
// ============================================================================
    IcJsonRpc2Result :: IcJsonRpc2Result ( const IcJsonRpc2Result & other )
{
    gRegJrs(); m_obj = nullptr;
    IcJsonRpc2ResultPriv::attach( & m_obj, T_PtrCast( void**, & other.m_obj));
}

// ============================================================================
// ctor ( copy assign )
// ============================================================================
IcJsonRpc2Result &   IcJsonRpc2Result :: operator =
                                         ( const IcJsonRpc2Result &other )
{
    IcJsonRpc2ResultPriv::attach( & m_obj, T_PtrCast( void**, & other.m_obj));
    return *this;
}

// ============================================================================
// methods wrapper
// ============================================================================
bool         IcJsonRpc2Result :: isNull() const
{ return ( m_obj == nullptr ); }

QJsonObject  IcJsonRpc2Result :: make() const
{ return ( m_obj != nullptr ? T_PrivRsl( m_obj )->make() : QJsonObject()); }

int          IcJsonRpc2Result :: id() const
{ return ( m_obj != nullptr ? T_PrivRsl( m_obj )->id() : 0 ); }

QString      IcJsonRpc2Result :: idStr() const
{ return ( m_obj != nullptr ? T_PrivRsl( m_obj )->idStr() : QString()); }

QJsonObject  IcJsonRpc2Result :: result() const
{ return this->resultJv().toObject(); }

QJsonValue   IcJsonRpc2Result :: resultJv() const
{ return ( m_obj != nullptr ? T_PrivRsl( m_obj )->result() : QJsonValue()); }

bool    IcJsonRpc2Result :: isIdStr ( ) const
{ return ( m_obj != nullptr ? T_PrivRsl( m_obj )->isIdStr() : false ); }

void    IcJsonRpc2Result :: setId ( int id )
{ IcJsonRpc2ResultPriv::instanceCow( & m_obj )->setId( id ); }

void    IcJsonRpc2Result :: setId ( const QString &id )
{ IcJsonRpc2ResultPriv::instanceCow( & m_obj )->setId( id ); }

void    IcJsonRpc2Result :: setResult( const QJsonObject &jo )
{ IcJsonRpc2ResultPriv::instanceCow( & m_obj )->setResult( jo ); }

void    IcJsonRpc2Result :: setResult( const QJsonValue &jv )
{ IcJsonRpc2ResultPriv::instanceCow( & m_obj )->setResult( jv ); }

bool    IcJsonRpc2Result :: loadFrom ( const QByteArray &ba )
{ return this->loadFrom( QJsonDocument::fromJson( ba ).object()); }

bool    IcJsonRpc2Result :: loadFrom ( const QJsonObject &jo )
{
    return (
        ( !jo.isEmpty() )  ?
        IcJsonRpc2ResultPriv::instanceCow( & m_obj )->loadFrom( jo ) :
        false
    );
}

// ============================================================================
// [ static ] used to generated a json object
// ============================================================================
IcJsonRpc2Result  IcJsonRpc2Result :: makeResult (
        const QxPack::IcJsonRpc2Request &req, const QJsonValue &jv
) {
    IcJsonRpc2Result rsl;
    if ( ! req.isNotification() ) {
        if ( req.isIdStr() ) {
            rsl = IcJsonRpc2Result( req.idStr(), jv );
        } else {
            rsl = IcJsonRpc2Result( req.id(), jv );
        }
    }
    return rsl;
}


// ////////////////////////////////////////////////////////////////////////////
//
//                IcJsonRpc2Error
//
// ////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////
//  IcJsonRpc2Error private objects
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
/*!
 * @brief get the error message by code
 * @param c [in] code
 * @return if 'c' is pre-defined code, return the pre-defined message, \n
 *   otherwise return empty string.
 */
// ============================================================================
static QString  gQueryErrMsgByCode( int c )
{
    QString msg;
    switch( c ) {
    case -32600 : msg = QStringLiteral("Invalid Request");  break;
    case -32601 : msg = QStringLiteral("Method not found"); break;
    case -32602 : msg = QStringLiteral("Invalid params");   break;
    case -32603 : msg = QStringLiteral("Internal error");   break;
    case -32700 : msg = QStringLiteral("Parse error");      break;
    case -32000 : msg = QStringLiteral("Too many requests");break;
    case -33001 : msg = QStringLiteral("Unrecognized response"); break;
    default: break;
    }
    return msg;
}

#define T_PrivErr( o )  T_ObjCast( IcJsonRpc2ErrorPriv *, o )
class  QXPACK_IC_HIDDEN IcJsonRpc2ErrorPriv :
                       public IcPImplPrivTemp< IcJsonRpc2ErrorPriv >
{
private:
    QJsonValue  m_error_data;  QString  m_error_msg, m_id_str;
    int         m_id;          int      m_error_code;
public:
    IcJsonRpc2ErrorPriv ( ) { m_id = 0; m_error_code = 0;  }
    IcJsonRpc2ErrorPriv ( const IcJsonRpc2ErrorPriv &other )
    {
        m_id = other.m_id; m_id_str = other.m_id_str;
        m_error_code = other.m_error_code;
        m_error_msg  = other.m_error_msg;
        m_error_data = other.m_error_data;
    }
    virtual ~IcJsonRpc2ErrorPriv( );

    inline int          id( )         { return m_id; }
    inline QString      idStr( )      { return m_id_str; }
    inline int          errorCode( )  { return m_error_code; }
    inline QString      errorMessage(){ return m_error_msg;  }
    inline QJsonObject  errorData( )  { return m_error_data.toObject(); }
    inline QJsonValue   errorDataJv() { return m_error_data; }

    inline bool  isIdStr( ) const
    { return ( isIdNull() ? false : ! m_id_str.isEmpty() ); }

    inline bool  isIdNull( ) const
    {
        return ( m_error_code == IcJsonRpc2Error::Code_ParseError ||
                 m_error_code == IcJsonRpc2Error::Code_InvalidRequest );
    }

    inline void  setId ( int id )  { m_id = id; m_id_str = QString();}
    inline void  setId ( const QString &id ) { m_id_str = id; }
           void  setErrorCode   ( int c );
    inline void  setErrorMessage( const QString &e )     { m_error_msg  = e; }
    inline void  setErrorData   ( const QJsonValue &d )  { m_error_data = d; }

    QJsonObject  make ( ) const;
    bool         loadFrom ( const QJsonObject & );
};

// ============================================================================
// dtor
// ============================================================================
    IcJsonRpc2ErrorPriv :: ~IcJsonRpc2ErrorPriv( )
{

}

// ============================================================================
// make the data into QJsonObject
// ============================================================================
QJsonObject  IcJsonRpc2ErrorPriv :: make ( ) const
{
    QJsonObject jo;
    jo.insert( QStringLiteral("jsonrpc"), QStringLiteral("2.0"));

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: id must be null if parse error or invalid request
    // ------------------------------------------------------------------------
    if ( m_error_code == IcJsonRpc2Error::Code_InvalidRequest ||
         m_error_code == IcJsonRpc2Error::Code_ParseError )
    {
        jo.insert( QStringLiteral("id"), QJsonValue(QJsonValue::Null));
    } else {
        jo.insert(
            QStringLiteral("id"),
            ( isIdStr() ? QJsonValue(m_id_str) : QJsonValue(m_id ))
        );
    }

    QJsonObject err;
    err.insert( QStringLiteral("code"),    m_error_code );
    err.insert( QStringLiteral("message"), m_error_msg  );
    if ( ! m_error_data.isUndefined() && ! m_error_data.isNull() ) { // 2019/04/06 fixed
        err.insert( QStringLiteral("data"), m_error_data );
    }
    jo.insert( QStringLiteral("error"), err );
    return jo;
}

// ============================================================================
// load data from byte array stream
// ============================================================================
bool    IcJsonRpc2ErrorPriv :: loadFrom( const QJsonObject & jo )
{
    // ------------------------------------------------------------------------
    // check header
    // ------------------------------------------------------------------------
    if ( jo.isEmpty() )     { return false; }
    if ( jo.value("jsonrpc").toString() != QStringLiteral("2.0")) {
        return false;
    }
    QJsonValue jv; bool is_chk_ok = false;

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "id" can be string, int, or null
    // ------------------------------------------------------------------------
    jv = jo.value("id");
    int id = 0; QString id_str; bool id_is_null = false; is_chk_ok = false;
    if      ( jv.isDouble() ) {
        id = jv.toInt(0);       is_chk_ok = true;
    } else if ( jv.isString() ) {
        id_str = jv.toString(); is_chk_ok = ! id_str.isEmpty();
    } else if ( jv.isNull()   ) {
        id_is_null = true;      is_chk_ok = true;
    } else { }

    if ( ! is_chk_ok ) { return false; }

    // ------------------------------------------------------------------------
    // JsonRpc2 spec.: "error" object must be existed.
    // ------------------------------------------------------------------------
    jv = jo.value("error");
    if ( ! jv.isObject()) { return false; }
    QJsonObject err_obj = jv.toObject();

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "code" in error object must be existed. Should be int.
    // ------------------------------------------------------------------------
    jv = err_obj.value("code");
    int err_code = 0; is_chk_ok = false;
    if ( jv.isDouble()) {
        err_code = jv.toInt(0); is_chk_ok = true;
    } else { ; }

    if ( ! is_chk_ok ) { return false; }

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.:
    //  "id" should be null if code is invalid req. or parse error
    // ------------------------------------------------------------------------
    if ( id_is_null ) {
        if ( err_code != IcJsonRpc2Error::Code_ParseError &&
             err_code != IcJsonRpc2Error::Code_InvalidRequest
        ) { return false; }
    } else {
        if ( err_code == IcJsonRpc2Error::Code_ParseError ||
             err_code == IcJsonRpc2Error::Code_InvalidRequest
        ) { return false; } // id should be null
    }

    // ------------------------------------------------------------------------
    // JsonRpc2 Spec.: "message" should be a string.
    // ------------------------------------------------------------------------
    jv = err_obj.value("message");
    QString  err_msg;
    if ( jv.isString() ) { err_msg = jv.toString(); }
    else { return false; }

    // ------------------------------------------------------------------------
    // store data
    // ------------------------------------------------------------------------
    m_id = id; m_id_str = id_str;
    m_error_code = err_code;
    m_error_msg  = err_msg;
    m_error_data = err_obj.value("data"); // maybe undefined.
    return true;
}

// ============================================================================
// set the error code, if error code is pre-defined, automatic setup
// error message
// ============================================================================
void   IcJsonRpc2ErrorPriv :: setErrorCode( int c )
{
    m_error_code = c;    
    QString inner_msg = gQueryErrMsgByCode( c );
    if ( ! inner_msg.isEmpty() ) { m_error_msg = inner_msg; }
}

// ////////////////////////////////////////////////////////////////////////////
// IcJsonRpc2Error wrapper API
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_jre_reg = false;
static void gRegJre( )
{
    if ( ! g_is_jre_reg ) {
        qRegisterMetaType<QxPack::IcJsonRpc2Error>( ); g_is_jre_reg = true;
    }
}

// ============================================================================
// ctor ( create empty object )
// ============================================================================
IcJsonRpc2Error :: IcJsonRpc2Error ( )
{ gRegJre(); m_obj = nullptr; }

// ============================================================================
// ctor ( create by code )
// ============================================================================
IcJsonRpc2Error :: IcJsonRpc2Error (
    int code, int id, const QJsonObject &d, const QString &ext_msg
) {
    gRegJre(); m_obj = nullptr;
    IcJsonRpc2ErrorPriv::buildIfNull( & m_obj );
    T_PrivErr( m_obj )->setId       ( id   );
    T_PrivErr( m_obj )->setErrorData( d    );
    T_PrivErr( m_obj )->setErrorCode( code );
    if ( ! ext_msg.isEmpty() ) {
        T_PrivErr( m_obj )->setErrorMessage( ext_msg );
    }
}

IcJsonRpc2Error :: IcJsonRpc2Error (
    int code, const QString &id, const QJsonObject &d, const QString &ext_msg
) {
    gRegJre(); m_obj = nullptr;
    IcJsonRpc2ErrorPriv::buildIfNull( & m_obj );
    T_PrivErr( m_obj )->setId       ( id   );
    T_PrivErr( m_obj )->setErrorData( d    );
    T_PrivErr( m_obj )->setErrorCode( code );
    if ( ! ext_msg.isEmpty() ) {
        T_PrivErr( m_obj )->setErrorMessage( ext_msg );
    }
}

IcJsonRpc2Error :: IcJsonRpc2Error ( // nw: 2019/04/03 added
    int code, int id, const QJsonValue &data, const QString &ext_msg
) {
    gRegJre(); m_obj = nullptr;
    IcJsonRpc2ErrorPriv::buildIfNull( & m_obj );
    T_PrivErr( m_obj )->setId ( id );
    T_PrivErr( m_obj )->setErrorData( data );
    T_PrivErr( m_obj )->setErrorCode( code );
    if ( ! ext_msg.isEmpty() ) {
        T_PrivErr( m_obj )->setErrorMessage( ext_msg );
    }
}

IcJsonRpc2Error :: IcJsonRpc2Error ( // nw: 2019/04/03 added
    int code, const QString &id, const QJsonValue &data, const QString &ext_msg
) {
    IcJsonRpc2ErrorPriv::buildIfNull( & m_obj );
    T_PrivErr( m_obj )->setId ( id );
    T_PrivErr( m_obj )->setErrorData( data );
    T_PrivErr( m_obj )->setErrorCode( code );
    if ( ! ext_msg.isEmpty() ) {
        T_PrivErr( m_obj )->setErrorMessage( ext_msg );
    }
}

// ============================================================================
// dtor
// ============================================================================
    IcJsonRpc2Error :: ~IcJsonRpc2Error( )
{
    if ( m_obj != nullptr ) { IcJsonRpc2ErrorPriv::attach( & m_obj, nullptr); }
}

// ============================================================================
// ctor ( create by another object )
// ============================================================================
    IcJsonRpc2Error :: IcJsonRpc2Error ( const IcJsonRpc2Error &other )
{
    gRegJre(); m_obj = nullptr;
    IcJsonRpc2ErrorPriv::attach( &m_obj, T_PtrCast( void**, & other.m_obj ));
}

// ============================================================================
// ctor ( copy assign )
// ============================================================================
IcJsonRpc2Error &  IcJsonRpc2Error :: operator =
                                      ( const IcJsonRpc2Error & other )
{
    IcJsonRpc2ErrorPriv::attach( &m_obj, T_PtrCast( void**, & other.m_obj ));
    return *this;
}

// ============================================================================
// methods wrapper
// ============================================================================
bool         IcJsonRpc2Error :: isNull() const
{ return ( m_obj == nullptr );  }

QJsonObject  IcJsonRpc2Error :: make( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->make() : QJsonObject()); }

bool         IcJsonRpc2Error :: isIdStr ( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->isIdStr() : false ); }

bool         IcJsonRpc2Error :: isIdNull( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->isIdNull() : false ); }

int          IcJsonRpc2Error :: id( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->id() : 0 ); }

QString      IcJsonRpc2Error :: idStr( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->idStr() : QString()); }

int          IcJsonRpc2Error :: errorCode  ( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->errorCode() : 0 ); }

QString      IcJsonRpc2Error :: errorMessag( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->errorMessage() : QString());}

QJsonObject  IcJsonRpc2Error :: errorData  ( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->errorData():QJsonObject());}

QJsonValue   IcJsonRpc2Error :: errorDataJv( ) const
{ return ( m_obj != nullptr ? T_PrivErr( m_obj )->errorDataJv() : QJsonValue()); }

void  IcJsonRpc2Error :: setId ( int id )
{ IcJsonRpc2ErrorPriv::instanceCow( & m_obj )->setId( id ); }

void  IcJsonRpc2Error :: setId ( const QString &id )
{ IcJsonRpc2ErrorPriv::instanceCow( & m_obj )->setId( id ); }

void  IcJsonRpc2Error :: setErrorCode ( int c )
{ IcJsonRpc2ErrorPriv::instanceCow( & m_obj )->setErrorCode( c ); }

void  IcJsonRpc2Error :: setErrorMessage ( const QString &m )
{ IcJsonRpc2ErrorPriv::instanceCow( & m_obj )->setErrorMessage( m ); }

void  IcJsonRpc2Error :: setErrorData ( const QJsonObject &d )
{ IcJsonRpc2ErrorPriv::instanceCow( & m_obj )->setErrorData ( d ); }

bool  IcJsonRpc2Error :: loadFrom ( const QByteArray &ba )
{ return this->loadFrom( QJsonDocument::fromJson( ba ).object()); }

bool  IcJsonRpc2Error :: loadFrom ( const QJsonObject &jo )
{
    if ( ! jo.isEmpty() ) {
        return IcJsonRpc2ErrorPriv::instanceCow( & m_obj )->loadFrom( jo );
    } else {
        return false;
    }
}

// ============================================================================
// [ static ] make an error object
// ============================================================================
IcJsonRpc2Error IcJsonRpc2Error :: makeError (
    const IcJsonRpc2Request &req, int code, const QJsonObject &data,
    const QString &ext_msg
) {
    IcJsonRpc2Error err;
    if ( ! req.isNotification() ) {
        if ( req.isIdStr()) {
            err = IcJsonRpc2Error( code, req.idStr(), data, ext_msg );
        } else {
            err = IcJsonRpc2Error( code, req.id(), data, ext_msg );
        }
    }
    return err;
}

// ////////////////////////////////////////////////////////////////////////////
//
//                        Response object
//
// ////////////////////////////////////////////////////////////////////////////
static bool g_is_resp_reg = false;
static void gRegResp( )
{
    if ( ! g_is_resp_reg ) {
        qRegisterMetaType<QxPack::IcJsonRpc2Response>(); g_is_resp_reg = true;
    }
}

// ============================================================================
// ctor ( create empty object )
// ============================================================================
IcJsonRpc2Response :: IcJsonRpc2Response ( )
{ gRegResp(); m_ext = nullptr; }

// ============================================================================
// dtor
// ============================================================================
IcJsonRpc2Response :: ~IcJsonRpc2Response ( ) { }

// ============================================================================
// ctor ( create by another object )
// ============================================================================
IcJsonRpc2Response :: IcJsonRpc2Response ( const IcJsonRpc2Response &other)
{
    gRegResp();
    m_rsl = other.m_rsl; m_err = other.m_err; m_req = other.m_req;
    m_ext = nullptr;
}

IcJsonRpc2Response :: IcJsonRpc2Response ( const IcJsonRpc2Result &rsl )
{
    gRegResp(); m_rsl = rsl; m_ext = nullptr;
}

IcJsonRpc2Response :: IcJsonRpc2Response ( const IcJsonRpc2Error &err )
{
    gRegResp(); m_err = err; m_ext = nullptr;
}

IcJsonRpc2Response :: IcJsonRpc2Response ( const IcJsonRpc2Request &req )
{
    gRegResp(); m_req = req; m_ext = nullptr;
}

// ============================================================================
// copy assign ( assign another object )
// ============================================================================
IcJsonRpc2Response &  IcJsonRpc2Response :: operator =
    ( const IcJsonRpc2Response &other )
{
    m_rsl = other.m_rsl; m_err = other.m_err; m_req = other.m_req;
    m_ext = nullptr;
    return *this;
}

IcJsonRpc2Response &  IcJsonRpc2Response :: operator =
    ( const IcJsonRpc2Result &rsl )
{
    m_rsl = rsl; m_err = IcJsonRpc2Error(); m_req = IcJsonRpc2Request();
    m_ext = nullptr;
    return *this;
}

IcJsonRpc2Response &  IcJsonRpc2Response :: operator =
    ( const IcJsonRpc2Error &err )
{
    m_rsl = IcJsonRpc2Result(); m_err = err; m_req = IcJsonRpc2Request();
    m_ext = nullptr;
    return *this;
}

IcJsonRpc2Response &  IcJsonRpc2Response :: operator =
    ( const IcJsonRpc2Request &req )
{
    m_rsl = IcJsonRpc2Result(); m_err = IcJsonRpc2Error(); m_req = req;
    m_ext = nullptr;
    return *this;
}

// ============================================================================
// methos access
// ============================================================================
bool  IcJsonRpc2Response :: isNull() const
{ return ( m_rsl.isNull() && m_err.isNull() && m_req.isNull() ); }

bool  IcJsonRpc2Response :: isResult() const
{ return ! m_rsl.isNull() && m_err.isNull() && m_req.isNull();   }

bool  IcJsonRpc2Response :: isError() const
{ return   m_rsl.isNull() && ! m_err.isNull()  && m_req.isNull(); }

bool  IcJsonRpc2Response :: isRequest() const
{ return   m_rsl.isNull()  &&  m_err.isNull()  &&  ! m_req.isNull(); }

IcJsonRpc2Result  IcJsonRpc2Response :: result() const  { return m_rsl; }
IcJsonRpc2Error   IcJsonRpc2Response :: error()  const  { return m_err; }
IcJsonRpc2Request IcJsonRpc2Response :: request() const { return m_req; }

// nw: 2019/05/18 added
QJsonObject  IcJsonRpc2Response :: make( ) const
{
     if ( isRequest() ) { return m_req.make(); }
     if ( isResult())   { return m_rsl.make(); }
     if ( isError())    { return m_err.make(); }
     return QJsonObject();
}

}

#endif
