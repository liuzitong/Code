// /////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_jsonrpc2
  @author  night wing
  @date    2018/09, 2019/02, 2019/03, 2019/04
  @brief   define the objects about Json RPC 2
  @version 0.2.1.0
  @deprecated this class is obsoleted, the Rpc should use JRpcPlat library.
  @verbatim
      <author>    <date>    <history memo.>
      nightwing  2019/02    fixed bugs refe. to JsonRpc2 Spec.
      nightwing  2019/03    IcJsonRpc2Response, added a JsonRpc2Request member
      nightwing  2019/04    IcJsonRpc2Error, added QJsonValue as error.data
  @endverbatim
*/
// /////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_JSONRPC2_HXX
#define QXPACK_IC_JSONRPC2_HXX

/*!
  @addtogroup  QxPack
  @{
*/
/*!
  @addtogroup  indcom
  @{
*/
/*!
  @addtogroup  sys_Exported_Classes
  @{
*/

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QString>
#include <QMetaType>
#include <QVector>
#include "qxpack/indcom/common/qxpack_ic_def.h"

namespace  QxPack {

// ////////////////////////////////////////////////////////////////////////////
// pre-declare
// ////////////////////////////////////////////////////////////////////////////
class IcJsonRpc2Request;
class IcJsonRpc2Error;
class IcJsonRpc2Result;
class IcJsonRpc2Response;

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief IcJsonRpc2 Result object
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcJsonRpc2Result {
public:
    //! create an empty object
    IcJsonRpc2Result ( );

    //! create result by int id
    IcJsonRpc2Result ( int id,  const QJsonValue &rsl );

    //! create result by string id
    IcJsonRpc2Result ( const QString &id, const QJsonValue &rsl );

    //! copy ctor ( create object by another )
    IcJsonRpc2Result ( const IcJsonRpc2Result & );

    //! assign ctor
    IcJsonRpc2Result &  operator = ( const IcJsonRpc2Result & );

    //! dtor
    virtual ~IcJsonRpc2Result( );

    //! @return true means a null object
    bool         isNull ( ) const;

    //! @return make all member as  json object
    QJsonObject  make ( ) const;

    //! @return "id" integer value if isIdStr() return false
    //! @details \n
    //! if id is string type, this will return 0.\n
    //! user should use isIdStr() to check if it is integer value
    int          id ( ) const;

    //! @return "id" string value if isIdStr() return true
    //! @details \n
    //! if id is integer type, this will return empty string. \n
    //! user should use isIdStr() to check if it is string value
    QString      idStr ( ) const;

    //! @return QJsonObject() result
    //! @deprecated use resultJv() instead
    QJsonObject  result  ( ) const;

    //! @return return value as QJsonValue
    QJsonValue   resultJv ( ) const;

    //! setup the integer id value
    //! @note After called, isIdStr() return false
    void  setId     ( int );

    //! setup the string id vallue
    //! @note After called, isIdStr() return true
    void  setId     ( const QString & );

    //! setup the result member
    //! @deprecated use setResult( const QJsonValue&) instead
    void  setResult ( const QJsonObject &jo );

    //! setup the result member
    void  setResult ( const QJsonValue  &jv );

    //! check if the id member is a string or not
    bool  isIdStr ( ) const;

    //! load member from string ( QJsonRPC2 format json string )
    bool  loadFrom ( const QByteArray & );

    //! load data from QJsonObject
    bool  loadFrom ( const QJsonObject & );

    //! make a result by request object's id member
    static IcJsonRpc2Result  makeResult (
        const QxPack::IcJsonRpc2Request &req, const QJsonValue &jv
    );

private:
    void *m_obj;
};




// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  IcJsonRpc2 Error object
 */
// ////////////////////////////////////////////////////////////////////////////
class  QXPACK_IC_API  IcJsonRpc2Error {
public:
    //! construct an empty object
    explicit IcJsonRpc2Error ( );

    //! create by integer id
    //! @param code [in] the error code
    //! @param id   [in] the request ID
    //! @param data [in] the error data
    //! @param ext_msg [opt_in] the user defined error message.
    //! @note if 'ext_msg' is not empty, will replace the default message
    //! @note if 'code' is Code_InvalidRequest or Code_ParseError, the \n
    //!      isIdNull() will return true regardless of the id value
    IcJsonRpc2Error (
        int code, int id, const QJsonObject &data = QJsonObject(),
        const QString &ext_msg = QString()
    );

    //! override
    IcJsonRpc2Error (
        int code, const QString &id, const QJsonObject &data = QJsonObject(),
        const QString &ext_msg = QString()
    );

    //! override
    IcJsonRpc2Error ( // nw: 2019/04/03 added
        int code, int id, const QJsonValue &data, const QString &ext_msg = QString()
    );

    //! override
    IcJsonRpc2Error ( // nw: 2019/04/03 added
        int code, const QString &id, const QJsonValue &data, const QString &ext_msg = QString()
    );

    //! copy constructor
    IcJsonRpc2Error ( const IcJsonRpc2Error & );

    //! assign copy
    IcJsonRpc2Error &  operator = ( const IcJsonRpc2Error & );

    //! dtor
    virtual ~IcJsonRpc2Error( );

    //! check if this object is null
    bool          isNull ( ) const;

    //! @return make all member into QJsonObject
    QJsonObject   make ( ) const;

    //! @return check if the id member is a string type
    //! @note if the id member is null, also return false.
    bool          isIdStr ( ) const;

    //! @return check if the id member is a null type
    bool          isIdNull ( ) const;

    //! @return the id integer value
    int           id ( ) const;

    //! @return the id string value
    //! @note if the id member is null, also return false.
    //! @details \n
    QString       idStr( ) const;

    //! @return the error code
    int           errorCode   ( ) const;

    //! @return the error message
    QString       errorMessag ( ) const;

    //! @return the error data
    //! @deprecated use errorDataJv() instead
    QJsonObject   errorData   ( ) const;

    //! @return the error data value
    QJsonValue    errorDataJv ( ) const;  // nw: 2019/04/03 added

    //! setup the id as integer value
    //! @note if error code is -32700 or -32600, the id is ignored
    void  setId ( int );

    //! setup the id as string value
    //! @note if error code is -32700 or -32600, the id is ignored
    void  setId ( const QString & );

    //! setup the error mesasge
    //! @note should call this after setErrorCode()
    void  setErrorMessage ( const QString & );

    //! setup the error adition data
    void  setErrorData    ( const QJsonObject & );

    //! setup the error code
    //! @details \n
    //! if the code is pre-defined error code( see Code enumeration ) \n
    //! the inner message is automatically setuped. If user want to  \n
    //! overwrite the message, should call setErrorMessage() again.
    void  setErrorCode    ( int );

    //! load member data from QJsonObject string ( JsonRPC2 )
    bool          loadFrom ( const QByteArray & );

    //! load member data from QJsonObject
    bool          loadFrom ( const QJsonObject & );

    //! make error object
    static IcJsonRpc2Error  makeError (
        const IcJsonRpc2Request &req, int code,
        const QJsonObject &data = QJsonObject(),
        const QString &ext_msg = QString()
    );

    enum Code { // pre-defined errors
        Code_ParseError     = -32700,
        Code_InvalidRequest = -32600,
        Code_MethodNotFound = -32601,
        Code_InvalidParams  = -32602,
        Code_InternalError  = -32603,
        Code_ServerError_Begin = -32000,
        Code_ServerError_TooManyRequests = -32000,
        Code_ServerError_End   = -32099,
        Code_User = -33000,
        Code_User_Unrecognized_Response = -33001
    };

private:
    void *m_obj;
};




// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief JsonRpc2 Request object
 * @details \n
 *    There are 2 type request: \n
 *    a) normal \n
 *       id     -- a integer value or a string \n
 *       method -- string  \n
 *       params -- omitted or jsonarray or jsonobject \n
 *    b) notification \n
 *       id     -- not existed in notification \n
 *       method -- string \n
 *       params -- omitted or jsonarray or jsonobject \n
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcJsonRpc2Request {
public:
   //! create an empty object
   explicit IcJsonRpc2Request( );

   //! create by json object
   IcJsonRpc2Request ( const QJsonObject & );

   /*!
    * @brief create a valid object
    * @param [in] id, the JsonRpc2 member 'id'
    * @param [in] me, the JsonRpc2 member 'method'
    * @param [in] params, the JsonRpc2 member 'params'
    * @note  if 'params' is empty, means no params
    */
   IcJsonRpc2Request( int id, const QString &me, const QJsonObject &params );
   IcJsonRpc2Request( int id, const QString &me, const QJsonArray  &params );

   //! create a valid object by id string )
   IcJsonRpc2Request(
       const QString &id, const QString &me, const QJsonObject &params
   );
   IcJsonRpc2Request(
       const QString &id, const QString &me, const QJsonArray &params
   );

   //! create a notificaton request
   IcJsonRpc2Request( const QString &me, const QJsonObject &params );
   IcJsonRpc2Request( const QString &me, const QJsonArray  &params );

   //! ctor ( copy )
   IcJsonRpc2Request( const IcJsonRpc2Request & );

   //! copy assign
   IcJsonRpc2Request &  operator = ( const IcJsonRpc2Request &other );

   virtual ~IcJsonRpc2Request ( );

   bool         isNull  ( ) const;
   QJsonObject  make    ( ) const;

   int          id       ( ) const; // make isNotificaiton() return false
   QString      idStr    ( ) const; // make isNotification() return false
   QString      method   ( ) const;
   QJsonObject  params   ( ) const;
   QJsonArray   paramsJa ( ) const;

   bool         isNotification  ( ) const;
   bool         isIdStr         ( ) const;
   bool         isParamsJa      ( ) const;
   bool         isWaitForResult ( ) const; // Notification() need not to wait

   void  setId     ( int );
   void  setId     ( const QString & );
   void  setParams ( const QJsonObject & );
   void  setParams ( const QJsonArray  & );
   void  setMethod ( const QString &     );
   void  setIsWaitForResult ( bool ); // deprecated. do nothing.
   void  setIsNotification  ( bool );

   //! load Json RPC2 data from a json string ( QJsonRPC2 )
   bool loadFrom ( const QByteArray &, IcJsonRpc2Error::Code *e = Q_NULLPTR );

   //! load json RPC2 data from json object
   bool loadFrom ( const QJsonObject &, IcJsonRpc2Error::Code *e = Q_NULLPTR );

private:
   void *m_obj;
};


// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief Response ( combined result and error )
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API IcJsonRpc2Response {
public:
    //! create empty object
    IcJsonRpc2Response ( );

    //! copy constructor
    IcJsonRpc2Response ( const IcJsonRpc2Response & );

    //! create by result
    IcJsonRpc2Response ( const IcJsonRpc2Result & );

    //! create by error
    IcJsonRpc2Response ( const IcJsonRpc2Error & );

    //! create by request
    IcJsonRpc2Response ( const IcJsonRpc2Request & ); // nw: 2019/03/29 added

    //! assign copy
    IcJsonRpc2Response &  operator = ( const IcJsonRpc2Response & );

    //! assign copy
    IcJsonRpc2Response &  operator = ( const IcJsonRpc2Result & );

    //! assign copy
    IcJsonRpc2Response &  operator = ( const IcJsonRpc2Error & );

    //! assign copy
    IcJsonRpc2Response &  operator = ( const IcJsonRpc2Request & ); // nw: 2019/03/29 added

    //! dtor
    virtual ~IcJsonRpc2Response ( );

    //! check if it is null
    bool  isNull( ) const;

    //! check if it is a result
    bool  isResult( ) const;

    //! check if it is a error
    bool  isError( )  const;

    //! check if it is a request from server...
    //! @note this is a custom extend, not the JsonRpc2 standard spec.
    bool  isRequest( ) const;

    //! return result
    IcJsonRpc2Result   result( ) const;

    //! return error
    IcJsonRpc2Error    error( ) const;

    //! return request
    IcJsonRpc2Request  request( ) const;

    //! directly return QJsonObject   nw: 2019/05/18 added
    QJsonObject   make( ) const;

private:
    IcJsonRpc2Result  m_rsl;
    IcJsonRpc2Error   m_err;
    IcJsonRpc2Request m_req;
    void *m_ext;
};

}

Q_DECLARE_METATYPE(QxPack::IcJsonRpc2Request)
Q_DECLARE_METATYPE(QxPack::IcJsonRpc2Error)
Q_DECLARE_METATYPE(QxPack::IcJsonRpc2Result)
Q_DECLARE_METATYPE(QxPack::IcJsonRpc2Response)
Q_DECLARE_METATYPE(QVector<QxPack::IcJsonRpc2Request>)
Q_DECLARE_METATYPE(QVector<QxPack::IcJsonRpc2Response>)

/*!
  @}
*/
/*!
  @}
*/
/*!
  @}
*/

#endif
