// ////////////////////////////////////////////////////////////////////////////
/*!
  @file    qxpack_ic_jotaskworker
  @author  night wing
  @date    2018/09, 2019/05
  @version 0.2.0.0
*/
// ////////////////////////////////////////////////////////////////////////////
#ifndef QXPACK_IC_JOTASKWORKER_HXX
#define QXPACK_IC_JOTASKWORKER_HXX

#include <QObject>
#include <QJsonObject>
#include "qxpack/indcom/common/qxpack_ic_def.h"
#include "qxpack/indcom/sys/qxpack_ic_vardata.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief    json task worker [ thread-safe ]
 * @details  this object manage the posted task item, all item processed by parallel.
 * @warning  all task are no order !
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcJoTaskWorker : public QObject {
    Q_OBJECT
public:
    //! the task handler type
    enum HandlerType {
        HandlerType_Routine = 0, //!< normal do the task, callback should do routine and free the resource
        HandlerType_Drop    = 1  //!< the task need to be dropped, callback should free the resource
    };

    //! task handler proto
    typedef void (* TaskHandler )( void *ctxt, const QJsonObject &jo, const IcVarData &v, HandlerType );

    //! ctor with shared qthread pool
    IcJoTaskWorker( QObject *pa = Q_NULLPTR );

    //! ctor with new qthread pool
    IcJoTaskWorker( int cr_thread_pool_max_item_num, QObject *pa = Q_NULLPTR );

    //! prepare the exit, once setup it, the IcJoTaskWorker() is shutdown.
    void    shutdown( );

    /*!
     * @brief wait for all posted or send item done
     * @param [in] ms, how many millionseconds to wait
     * @return true for all items are done.
     */
    bool     waitForAllDone( int ms = -1 );

    /*!
     * @brief destruct the json task manager
     * @note  if exist task in the manager, these task will be called with HandlerType_Drop
     */
    virtual ~IcJoTaskWorker( ) Q_DECL_OVERRIDE;

    /*!
     * @brief post a task handler to execute
     * @param [in] task_handler, the callback function defined by user
     * @param [in] task_ctxt,    the callback function will pass back it
     * @param [in] jo,           the json descriptor
     * @param [in] ext_param,    extend parameter pointer
     * @return true for OK;
     * @note  all parameter will be cached in manager.
     */
    bool   post(
        TaskHandler task_handler, void *task_ctxt,
        const QJsonObject &jo,    const IcVarData &v = IcVarData()
    );

    /*!
     * @brief same as post(), but it will be wait for it begin to execute
     * @note the last parameter 'wait_for_finish' means whether wait \n
     *      for the callback finished.
     */
    bool   send(
        TaskHandler task_handler, void *task_ctxt,
        const QJsonObject &jo,    const IcVarData &v = IcVarData(),
        bool wait_for_finish = true
    );


private:
    void *m_obj;
    Q_DISABLE_COPY( IcJoTaskWorker )
};



}

#endif
