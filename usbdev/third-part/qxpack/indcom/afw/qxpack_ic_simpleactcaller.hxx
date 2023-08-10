// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_simpleactcaller
  @author  night wing
  @date    2019/01, 2019/05
  @version 0.2.0.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_SIMPLEACTCALLER_HXX
#define QXPACK_IC_SIMPLEACTCALLER_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QVariantList>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  simple action caller
 * @details \n
 *     The Action, is a descriptor of an method. \n
 *     A method, has some states:  "state_freeze", "state_idle", "state_working". \n
 *     "state_freeze"  -- the method forbid to execute. \n
 *     "state_idle"    -- the method call be called, then the state changed to "state_working". \n
 *     "state_working" -- the method is working, if it is in this state, can not call it again. \n
 *  \n
 *     There are diffrient methods: \n
 *     "type_trigger"  -- this type has two states: "state_freeze", "state_idle" \n
 *     "type_switch"   -- this type has all states: "state_freeze", "state_working", "state_idle" \n
 *  \n
 * @warning
 *    this is diffirent from IcActionExec, this will only run in CALLER THREAD. \n
 *
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcSimpleActCaller : public QObject {
    Q_OBJECT
public:
    enum State {
        State_Freeze = 0, State_Idle = 1, State_Working = 2
    };

    //! ctor
    explicit IcSimpleActCaller( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcSimpleActCaller( ) Q_DECL_OVERRIDE;

    /*!
     * @brief auto build the action-state network inner.
     * @param  cb_obj   [in] callback object, that must has Q_SLOT/Q_INVOKABLE protocol function
     * @param  req_evt_loop [in] if in diffirent thread, this option is used.
     * @return true for build network OK, otherwise means failed.
     * @verbatim
     *     for switch:
     *         Q_SLOT bool  onSw_<action name >( const QJsonObject &jo, const QVariantList &va, int curr_state, int tgt_state );
     *     for trigger:
     *         Q_SLOT bool  onTrg_<action name> ( const QJsonObject &jo, const QVariantList &va, int curr_state, int tgt_state );
     *      the default implement is return true.
     * @endverbatim
     * @note  switch or trigger can only has one same method name, or will be fatal error.
     * @warning the 'cb_obj' must be alived while IcSimpleActCaller is alive.
    */
    Q_INVOKABLE bool  build ( QObject *cb_obj, bool req_evt_loop = true );

    /*!
     * @brief post a action into queue.
     * @param  act_name  [in]  the action name
     * @param  jo        [in]  parameter described by Json
     * @param  ext_param [in] extend parameter pointer
     * @return true for posted.
     * @note  If existed a action thread, this operation post the request \n
     *        and return immediately. Otherwise, this operation queued in caller \n
     *        event queue.
     */
    Q_INVOKABLE bool  post (
        const QString &act_name, const QJsonObject &jo, const QVariantList &ext_param = QVariantList()
    );

    //! post the cancel request
    Q_INVOKABLE bool  postCancel ( const QString &act_name );

    //! post a freeze request
    Q_INVOKABLE bool  postFreeze ( const QString &act_name );

    //! post a unfreeze request
    Q_INVOKABLE bool  postUnfreeze( const QString &act_name );

    //! blocking barrier call
    //! @return  this call is a sync. call.
    Q_INVOKABLE void  blockBarrier( bool req_evt_loop = true );

    // nw: 2019/05/03 added
    /*!
     * @brief send a action to do
     * @param  act_name  [in]  the action name
     * @param  jo        [in]  parameter described by Json
     * @param  ext_param [in] extend parameter pointer
     * @return true for called
     * @note  If existed a action thread, this operation post the request \n
     *        and wait for it completed.
     */
    Q_INVOKABLE bool   send (
        const QString &act_name, const QJsonObject &jo, const QVariantList &ext_param = QVariantList()
    );

    // nw: 2019/05/03 added
    //! send the cancel request
    Q_INVOKABLE bool   sendCancel ( const QString &act_name );

    // nw: 2019/05/03 added
    //! send a freeze request
    Q_INVOKABLE bool   sendFreeze ( const QString &act_name );

    // nw: 2019/05/03 added
    //! send a unfreeze request
    Q_INVOKABLE bool   sendUnfreeze ( const QString &act_name );

    //! emit by a action state changed.
    Q_SIGNAL void  stateChanged ( const QString &act_name, int curr_state, int target_state );

private:
    void *m_obj;
    Q_DISABLE_COPY( IcSimpleActCaller )
};

}

#endif
