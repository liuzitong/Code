#ifndef QXPACK_IC_SIMPLEFSM_HXX
#define QXPACK_IC_SIMPLEFSM_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QVariantList>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  a simple Finite State Machine driver
 * @details used to perform simple finite state transition, the FSM will drive \n
 *    the logic of the state graph. \n
 *    A state that has 3 functions: \n
 *       void  onFsm_<State>_Enter(), this function will be called while FSM enter this state. \n
 *       void  onFsm_<State>_Leave(), this function will be called while FSM leave this state. \n
 *       void  onFsm_<State>_Routine(), this function is used to detect the event, called repeatly by user.\n
 *    There are two method to do state transition: \n
 *    a) use postGo() to set next state. \n
 *    b) use postEvent() to do a event callback, but the return value is important, \n
 *    true means event transition OK, go next state.
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcSimpleFsm : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcSimpleFsm )
public:

    //! ctor
    explicit IcSimpleFsm ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcSimpleFsm ( ) Q_DECL_OVERRIDE;

    //! initailize by spec. object
    //! @param h_obj       [in]  the handler object that contains some functions
    //! @param init_state  [in]  the start state name
    //! @param last_state  [in]  the finish state name
    //! @param cancel_state [opt-in]  the cancel state name
    //! @return true for build State Network by handler object
    //! @warning the h_obj should be same thread with SimpleFSM
    //! @note  the available format of state callback functions is below: \n
    //!   Q_SLOT  void  onFsm_<State>_Enter() \n
    //!   Q_SLOT  void  onFsm_<State>_Leave() \n
    //!   Q_SLOT  void  onFsm_<State>_Routine( const QVariantList & ) \n
    //!   Q_SLOT  bool  onFsm_<SrcState>_<Event>_<DstState>( const QVariantList& ) \n
    Q_SLOT  bool  init ( // old..
        QObject *h_obj, const QString &init_state, const QString &last_state
    );
    Q_SLOT  bool  init ( // nw: 2019/06/21 added
        QObject *h_obj,
        const QString &init_state, const QString &last_state, const QString &cancel_state
    );

    //! @return check if it is finished
    Q_SLOT  bool  isFinished( ) const;

    //! @return check if it is started
    Q_SLOT  bool  isStarted( ) const;

    //! @return check if it is cancelled
    Q_SLOT  bool  isCancelled( ) const; // nw: 2019/06/21 added

    //! start the FSM
    //! @return true means posted
    //! @note this call will reset the state to initialize state. This step is needed before FSM working.\n
    Q_SLOT  bool  postStart();

    //! post a event to FSM
    //! @param  evt_name  [in] the event name
    //! @param  par       [in] parameter that will be passed to state handler
    //! @return true means posted
    //! @note  post a event to trigger state transition.
    //! @note  'par' need to do a assign copy due to the FSM will do it in next event loop.
    Q_SLOT  bool  postEvent ( const QString &evt_name, const QVariantList &par = QVariantList());

    //! post directly to target state
    //! @return true means posted
    //! @note this will directly jump current state to target state, the triggered functions order is \n
    //!   <Src_State>_Leave(), <Tgt_State>_Enter()
    //! @note the action will do in next event loop
    Q_SLOT  bool  postGo ( const QString &tgt_state );

    //! post do current state routine call.
    //! @return true means posted
    //! @note the action will do in next event loop
    Q_SLOT  bool  postRoutine ( const QVariantList & = QVariantList());



    //!  emited while started
    Q_SIGNAL void  started( );

    //!  emited while reach last state
    Q_SIGNAL void  finished( );

    //!  emited while reach cancel state
    Q_SIGNAL void  cancelled( );

    //!  unhandle event on state
    //! @details the event can not found any transition function
    Q_SIGNAL void  unhandleEvent( const QString &evt_name, const QVariantList &par );

    //!  untransitioned event
    //! @details the event handled but function return false
    Q_SIGNAL void  untransitionedEvent( const QString &evt_name, const QVariantList &par );

    //!  the state is not existed
    Q_SIGNAL void  unexistedState( const QString &st );

private:
    void *m_obj;
};


// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief  the simple state machine template cache keeper
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcSimpleFsmTempCache : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY( IcSimpleFsmTempCache )
public:
    //! get the instance of current simple FSM cache
    static IcSimpleFsmTempCache*  getInstance( );

    //! free the instance of current simple FSM cache
    static void  freeInstance( );

    //! register a template
    void  regTemplate( const QMetaObject & );

    //! override regTemplate
    template <typename T>
    inline void  regTemplate( )
    { this->regTemplate( T :: staticMetaObject ); }

protected:
    //! dtor
    explicit IcSimpleFsmTempCache( QObject* = Q_NULLPTR );

    //! dtor
    virtual ~IcSimpleFsmTempCache( ) Q_DECL_OVERRIDE;

private:
    void *m_obj;
};

}

#endif
