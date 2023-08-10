#ifndef QXPACK_IC_EVENTSYS_HXX
#define QXPACK_IC_EVENTSYS_HXX

#include "qxpack_ic_def.h"
#include <cstdint>

namespace QxPack {

class IcEventLoop;

// ////////////////////////////////////////////////////////////////////////////
//
//                      the event object defined for eventloop
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcEvent {
public:
    typedef void (*DelCallback)( IcEvent*, void*ctxt );

    explicit IcEvent( unsigned short type, DelCallback cb, void *cb_ctxt = nullptr );
    virtual ~IcEvent();

    inline void  accept() { m_accept = true; }
    inline void  ignore() { m_accept = false;}

    inline unsigned short type() const { return m_type; }
    inline bool  isPrivSys() const  { return ( m_priv_sys != 0 ); }
    static void  dispose( IcEvent* );

protected:
    inline void   setPosted()  { m_posted   = true; }
    inline void   markPrivSys(){ m_priv_sys = true; }

    friend class IcEventLoop;
private:
    void  *m_ext;  DelCallback  m_del_cb; void *m_del_ctxt;
    unsigned short m_type;
    unsigned short m_posted : 1;
    unsigned short m_spont  : 1;
    unsigned short m_accept : 1;
    unsigned short reserved : 12;
    unsigned short m_priv_sys : 1;  

    QXPACK_IC_DISABLE_COPY( IcEvent )
};

// ////////////////////////////////////////////////////////////////////////////
//
//                      the eventloop
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcEventLoop QXPACK_IC_FINAL {
public:
    enum ProcEvtFlag { ProcEvtFlag_AllEvents  };

    explicit IcEventLoop ( void * = nullptr );

    ~IcEventLoop( );

    int   exec( ProcEvtFlag = ProcEvtFlag_AllEvents );

    int   exit( int ret_code = 0 );

    bool  isRunning() const;

    void  wakeUp();

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcEventLoop )
};





}
#endif
