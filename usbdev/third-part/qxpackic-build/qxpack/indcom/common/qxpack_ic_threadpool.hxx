#ifndef QXPACK_IC_THREADPOOL_HXX
#define QXPACK_IC_THREADPOOL_HXX

#include "qxpack_ic_def.h"
#include "qxpack_ic_logging.hxx"
#include "qxpack_ic_object.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//                      Runnable Interface
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcRunnable {
public :
    typedef void (* DelCallback )( IcRunnable *, void* );

    explicit IcRunnable( DelCallback dcb, void*ctxt ) { m_del_cb = dcb; m_del_ctxt = ctxt; m_is_auto_del = true; }
    virtual ~IcRunnable() { }

    inline  bool        autoDelete()  const { return m_is_auto_del; }
    inline  DelCallback delCallback() const { return m_del_cb;   }
    inline  void*       delCtxt() const     { return m_del_ctxt; }

    virtual void  run() = 0;

    inline  void setAutoDelete( bool auto_del ) { m_is_auto_del = auto_del; }

    static void dispose( IcRunnable *r )
    {
       if ( r != nullptr ){
          if ( r->m_is_auto_del ) {
              if ( r->m_del_cb != nullptr ) { r->m_del_cb( r, r->m_del_ctxt ); }
              else { qxpack_ic_fatal("IcRunnable obj 0x%x set auto delete but no callback function!", r ); }
          }
       }
    }
private:
    void *m_ext;
    DelCallback m_del_cb; void *m_del_ctxt;
    bool  m_is_auto_del;
};



// ////////////////////////////////////////////////////////////////////////////
// 
//                      Thread Pool 
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcThreadPool : public IcObject {

public :
    explicit IcThreadPool( void * = nullptr );

    //! @note it will blocking until all run object has been solved.
    virtual ~IcThreadPool( );    

    //! remove all waiting runnable objects
    void  clear( );

    //! \return a time that used to free unused threads
    int   expiryTimeout() const;

    //! \return maxinum thread count
    int   maxThreadCount() const;

    void  setExpiryTimeout( int tm );

    void  setMaxThreadCount( int );

    //! \return waiting for all runable object are done
    bool  waitForDone( int msec = -1 );

    //! \brief post a runnable object
    //! \warning this function take the ownership of runnable object!
    void  start( IcRunnable *r, int prior = 0 );    

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcThreadPool )
};

}
#endif
