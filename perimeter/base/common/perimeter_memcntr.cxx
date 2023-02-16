#ifndef PERIMETER_MEMCNTR_CXX
#define PERIMETER_MEMCNTR_CXX

#include "perimeter_memcntr.hxx"
#include "perimeter_logging.hxx"
#include "perimeter_semtemp.hpp"
#include "perimeter_dequetemp.hpp"
#include "perimeter_dyncinit_priv.hxx"

#ifdef PERIMETER_QT5_ENABLED
#  include <QDebug>
#  include <QThread>
#  include <QObject>
#  include <QMetaObject>
#endif

#include <atomic> // C++ 11
#include <thread> // C++ 11
#include <chrono> // C++ 11
#include <map>    // C++ 11
#include <functional> // C++ 11
#include <cstring>
#include <limits>
#include <cstdio>
#include <memory>

#if defined(WIN32)
#include <windows.h>
#endif

#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif


namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
// record. each new/delete request will be record here
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_HIDDEN MemCntrRec {
private: const char *m_file_ptr, *m_type_ptr; int m_cntr;
public :
    MemCntrRec( ) { m_file_ptr = m_type_ptr = nullptr; }
    MemCntrRec( const char *fp, const char *, const char *tp, int, int cnt )
    { m_file_ptr= fp; m_type_ptr = tp; m_cntr = cnt; }
    inline const char * filePtr( ) const { return m_file_ptr; }
    inline const char * typePtr( ) const { return m_type_ptr; }
    inline int          cntr( ) const    { return m_cntr; }
};

// ////////////////////////////////////////////////////////////////////////////
// const string comparator. compare the existed const string in program.
// ////////////////////////////////////////////////////////////////////////////
typedef  const char*  MemCntrConstStrPtr;
class PERIMETER_HIDDEN MemCntrConstStrCmp {
public:
    inline bool operator()( const MemCntrConstStrPtr &a, const MemCntrConstStrPtr &b ) const
    { return ( a == b ? false : ( std::strcmp( a, b ) < 0 ) ); }
};

// ////////////////////////////////////////////////////////////////////////////
// descriptor. describe the alloc for each file
// ////////////////////////////////////////////////////////////////////////////
typedef  std::map<MemCntrConstStrPtr,int,MemCntrConstStrCmp>  MemCntrRecDescrTypeMap;
class PERIMETER_HIDDEN MemCntrRecDescr {
private: MemCntrRecDescrTypeMap m_type_map;
public :
     MemCntrRecDescr( ) { }
    ~MemCntrRecDescr( ) { }
     MemCntrRecDescr( const MemCntrRecDescr &other ) { m_type_map = other.m_type_map; }
     MemCntrRecDescr &  operator = ( const MemCntrRecDescr & other ) { m_type_map = other.m_type_map; return *this; }
     inline MemCntrRecDescrTypeMap &  typeMap( ) { return m_type_map; }

    void  addCntr( int c, const char *type )
    {
        if ( type == nullptr ) { type = "std_mem_alloc"; }
        MemCntrRecDescrTypeMap::iterator itr = m_type_map.find( type );
        if ( itr == m_type_map.end()) {
            m_type_map.insert( std::pair<MemCntrConstStrPtr,int>( type, c ));
        } else {
            itr->second += c;
        }
    }
};

// ////////////////////////////////////////////////////////////////////////////
// action cb object
// ////////////////////////////////////////////////////////////////////////////
class PERIMETER_HIDDEN MemCntrRecAction {
private: void (* m_cb )(void*); void *m_ctxt;
public :
    MemCntrRecAction( ) { m_cb = nullptr; m_ctxt = nullptr; }
    MemCntrRecAction( const MemCntrRecAction &other )
    { m_cb = other.m_cb; m_ctxt = other.m_ctxt; }
    MemCntrRecAction & operator = ( const MemCntrRecAction &other )
    { m_cb = other.m_cb; m_ctxt = other.m_ctxt; return *this; }
    ~MemCntrRecAction( ) { }
    MemCntrRecAction ( void (*cb)(void*), void *ctxt )
    { m_cb = cb; m_ctxt = ctxt; }
    inline void call( ) { if ( m_cb != nullptr ) { (* m_cb )( m_ctxt ); }}
};

// ////////////////////////////////////////////////////////////////////////////
//
// record thread
//
// ////////////////////////////////////////////////////////////////////////////
typedef std::map<MemCntrConstStrPtr,MemCntrRecDescr,MemCntrConstStrCmp> MemCntrRecDescrMap;
typedef Perimeter::Deque<MemCntrRec>        MemCntrRecQueue;
typedef Perimeter::Deque<MemCntrRecAction>  MemCntrRecActionQueue;

class PERIMETER_HIDDEN MemCntrRecThread {
private:
    SemTemp    m_sem, m_sem_exit;
    MemCntrRecQueue       m_rec_queue_ts;
    MemCntrRecActionQueue m_rec_act_queue_ts;
    MemCntrRecDescrMap    m_rec_descr_map_pr;
    std::thread   m_thread;
    std::atomic<int>  m_flag;
public :
    void  threadWork( );
    MemCntrRecThread ( ) : m_rec_queue_ts( nullptr, nullptr ), m_rec_act_queue_ts( nullptr, nullptr ), m_flag(0)
    { m_thread = std::thread([this](){ this->threadWork(); }); }

    ~MemCntrRecThread( )
    {
#if defined( WIN32 ) // in windows, vs2012 vs2013 will hange if call join()
        m_flag.fetch_add(1);  // 1 means req-exit, 2 means already existed
        m_sem.notify();       // tell thread to wake up
        m_sem_exit.wait();
        m_thread.join();
#else
        m_flag.fetch_add(1);  // 1 means req-exit, 2 means already existed
        m_sem.notify();       // tell thread to wake up
        if ( m_thread.joinable()) { m_thread.join(); }
#endif
    }
    void  procRec_nw ( const char *fp, const char *type, int cntr );
    void  saveToFile ( const char *fn );
protected:
    void  procRec_nw_Act_pr( );
    void  saveToFile_Act_pr( const char *fn );
};

// ============================================================================
// thread worker routine
// ============================================================================
void   MemCntrRecThread :: threadWork( )
{
    while( m_flag.load() < 1 ) {
        // wait for wakeup
        m_sem.wait(); if ( m_flag.load() > 0 ) { break; }

        // read a action from action queue, and execute it
        MemCntrRecAction  act;
        if ( ! m_rec_act_queue_ts.pop( act ) ) { continue; }
        act.call( );
    }
    m_flag.fetch_add(1);
    m_sem_exit.notify(); // wakup the exit semaphore
#if defined( WIN32 )
    ::ExitThread(NULL);
#endif
}

// ============================================================================
// process the record ( no wait )
// ============================================================================
void   MemCntrRecThread :: procRec_nw( const char *fp, const char *type, int cntr )
{
    if ( fp == nullptr || type == nullptr ) { return; }
    m_rec_queue_ts.push( MemCntrRec( fp, nullptr,type, 0, cntr ) );
    m_rec_act_queue_ts.push( MemCntrRecAction(
                  [](void*ctxt){  perimeter_objcast(  MemCntrRecThread *, ctxt )->procRec_nw_Act_pr(); }, this
           ));
    m_sem.notify();
}
void   MemCntrRecThread :: procRec_nw_Act_pr( )
{
    MemCntrRec rec; if ( ! m_rec_queue_ts.pop( rec )) { return; }
    MemCntrRecDescrMap::iterator itr = m_rec_descr_map_pr.find( rec.filePtr() );
    if ( itr == m_rec_descr_map_pr.end()) {
        MemCntrRecDescr descr; descr.addCntr( rec.cntr(), rec.typePtr());
        m_rec_descr_map_pr.insert( std::pair<MemCntrConstStrPtr,MemCntrRecDescr>( rec.filePtr(),descr));
    } else {
        MemCntrRecDescr &d = itr->second;
        d.addCntr( rec.cntr(), rec.typePtr() );
    }
}

// ============================================================================
// save all information to a specified text file
// ============================================================================
void   MemCntrRecThread :: saveToFile( const char *fn )
{
    if ( fn == nullptr ) { return; }

    struct SaveToFileContxt {
        SemTemp m_sem; MemCntrRecThread *m_this; const char *m_fn;
    } contxt;
    contxt.m_this = this; contxt.m_fn = fn;

    m_rec_act_queue_ts.push( MemCntrRecAction(
                  [](void*ctxt){
                        SaveToFileContxt *st = perimeter_objcast( SaveToFileContxt *, ctxt );
                        st->m_this->saveToFile_Act_pr( st->m_fn );
                        st->m_sem.notify();
                  }, & contxt
           ));
    m_sem.notify();        // wake up
    contxt.m_sem.wait();   // wait for done
}

// save to file only save the none-zero counter records
void   MemCntrRecThread :: saveToFile_Act_pr( const char *fn )
{
    FILE *file = std::fopen( fn, "w");
    if ( file == nullptr ) { return; }
    std::fprintf(file,"=== Trace Information Output ===\n");
    MemCntrRecDescrMap::iterator f_itr = m_rec_descr_map_pr.begin();
    while ( f_itr != m_rec_descr_map_pr.end()) {
        MemCntrRecDescr &desc = f_itr->second;
        std::fprintf( file,"[ %s ]\n", f_itr->first ); // write the file header
        ++ f_itr;

        MemCntrRecDescrTypeMap &type_map = desc.typeMap();
        MemCntrRecDescrTypeMap::const_iterator t_itr = type_map.cbegin();
        while ( t_itr != type_map.cend()) {
            if ( t_itr->second != 0 ) {
                std::fprintf( file, "+ %s : %d\n", t_itr->first, t_itr->second );
            }
            ++ t_itr;
        }
        std::fprintf( file, "\n");
    }
    std::fflush( file );
    std::fclose( file );
}

// ////////////////////////////////////////////////////////////////////////////
//
//  recorder static factory
//
// ////////////////////////////////////////////////////////////////////////////
// the std::mutex will hange in dync.dtor stage or dync.ctor stage in DLL init/deInit
// ...here try to find some new way to support it..
// std::mutex, std::atomic.
static std::mutex        g_mem_cntr_rec_locker;
static std::atomic<MemCntrRecThread*> g_mem_cntr_rec_ptr( nullptr );
static std::atomic<int>  g_en_mem_trace(0);
static std::atomic<int>  g_en_mem_cntr(1);
static std::atomic<int>  g_atom(0);

static void  gInitRes  ( );
static void  gDeInitRes( );
static DyncInit  g_dync_init( & gInitRes, & gDeInitRes );

// ============================================================================
// make sure the resource init
// ============================================================================
static void  gInitRes( )
{
    static bool is_init = false;
    if ( ! *perimeter_ptrcast( volatile bool *, &is_init ) ) {
        is_init = true;
    }
}

// ============================================================================
// free the resource
// ============================================================================
static void  gDeInitRes( )
{
}

// ============================================================================
// check if memory trace is enabled or not
// ============================================================================
static inline bool  gIsEnableMemTrace( ) { return ( g_en_mem_trace.load() > 0 ); }

// ============================================================================
// check if memory counter is enabled or not
// ============================================================================
static inline bool  gIsEnableMemCntr( ) { return ( g_en_mem_cntr.load() > 0 ); }

// ============================================================================
// try build a record thread
// ============================================================================
static MemCntrRecThread *  gEnsureRecThread( )
{
    MemCntrRecThread *t = g_mem_cntr_rec_ptr.load();
    if ( t == nullptr ) {
        g_mem_cntr_rec_locker.lock();
        if ( g_en_mem_trace.load() > 0 ) { // re-check: makesure current is on
            if ( ( t = g_mem_cntr_rec_ptr.load()) == nullptr ) { // double check
                g_mem_cntr_rec_ptr.store( ( t = new MemCntrRecThread ));
            }
        }
        g_mem_cntr_rec_locker.unlock();
    } else {
        g_mem_cntr_rec_locker.lock();
        if ( g_en_mem_trace.load() < 1 ) {
            if ( ( t = g_mem_cntr_rec_ptr.load()) != nullptr ) {
                g_mem_cntr_rec_ptr.store( nullptr );
                delete t; t = nullptr;
            }
        }
        g_mem_cntr_rec_locker.unlock();
    }
    return t;
}

// ============================================================================
// enable memory trace
// ============================================================================
static inline void  gEnableMemTrace( bool sw ) {
    g_en_mem_trace.store( ( sw ? 1 : 0 ));
    gEnsureRecThread();
}

// ============================================================================
// enable memory counter
// ============================================================================
static inline void  gEnableMemCntr( bool sw ) {
    g_en_mem_cntr.store( ( sw ? 1 : 0 ));
}


// ////////////////////////////////////////////////////////////////////////////
//
//  Memory Counter wrap API
//
// ////////////////////////////////////////////////////////////////////////////
    MemCntr ::  MemCntr ( ) { }
    MemCntr :: ~MemCntr ( ) { }

// ============================================================================
// increase the new counter
// ============================================================================
int  MemCntr ::  incrNewCntr( int cntr, const char *file, const char *func, int ln, const char *type )
{
    int ct = 0;
    if ( gIsEnableMemCntr()) {
        ct = g_atom.fetch_add( cntr, std::memory_order_seq_cst ) + cntr;
    }
#if defined( PERIMETER_CFG_MEM_TRACE )
    if ( gIsEnableMemTrace() ) {
        MemCntrRecThread *t = gEnsureRecThread();
        if ( t != nullptr ) { t->procRec_nw( file, type, cntr ); }
        PERIMETER_UNUSED(func);
        PERIMETER_UNUSED(ln);
    }
#else
    PERIMETER_UNUSED(file); PERIMETER_UNUSED(func);
    PERIMETER_UNUSED(ln);   PERIMETER_UNUSED(type);
#endif
    return ct;
}

// ============================================================================
//  decrease the new counter
// ============================================================================
int  MemCntr :: decrNewCntr( int cntr, const char *file, const char *func, int ln, const char *type )
{
    int ct = 0;
    if ( gIsEnableMemCntr() ) {
        ct = g_atom.fetch_sub( cntr, std::memory_order_seq_cst ) - cntr;
    }
#if defined( PERIMETER_CFG_MEM_TRACE )
    if ( gIsEnableMemTrace()) {
        MemCntrRecThread *t = gEnsureRecThread();
        if ( t != nullptr ) { t->procRec_nw( file, type, -cntr ); }
        PERIMETER_UNUSED(func);
        PERIMETER_UNUSED(ln);
    }
#else
    PERIMETER_UNUSED(file); PERIMETER_UNUSED(func);
    PERIMETER_UNUSED(ln);   PERIMETER_UNUSED(type);
#endif
    return ct;
}

// ============================================================================
// save trace info to file
// ============================================================================
void   MemCntr :: saveTraceInfoToFile( const char *fn )
{
#if defined( PERIMETER_CFG_MEM_TRACE )
    if ( gIsEnableMemTrace()) {
        MemCntrRecThread *t = gEnsureRecThread();
        if ( t != nullptr ) { t->saveToFile( fn ); }
    }
#else
    PERIMETER_UNUSED( fn );
#endif

}

// ============================================================================
//  enable the memory trace
// ============================================================================
void MemCntr :: enableMemTrace( bool sw )
{

#if defined( PERIMETER_CFG_MEM_TRACE )
    gEnableMemTrace( sw );
#else
    PERIMETER_UNUSED( sw );
#endif
}

// ============================================================================
// enable memory counter
// ============================================================================
void  MemCntr :: enableMemCntr( bool sw )
{
    gEnableMemCntr( sw );
}

// ============================================================================
// return current new counter [ static ]
// ============================================================================
int  MemCntr :: currNewCntr( )
{
     return g_atom.load( std::memory_order_seq_cst );
}

// ============================================================================
// check the allocated pointer [ static ]
// ============================================================================
void*  MemCntr :: _assertNonNullPtr( void *p, bool is_alloc )
{
    if ( p == nullptr ) {
        if ( is_alloc ) {
            perimeter_fatal("allocated memory, but return nullptr!");
        } else {
            perimeter_fatal("free memory, pass nullptr!");
        }
    }
    return p;
}

// ============================================================================
// memory allocator
// ============================================================================
void*  MemCntr :: allocMemory ( size_t size )
{
    if ( size < 1 ) { size = 1; }
    char *p = perimeter_ptrcast( char *, std::malloc( size_t( size )));
    if ( p == nullptr ) {
        std::abort();
    }
    return p;
}

// ============================================================================
// re-allocate the memory
// ============================================================================
void*   MemCntr :: reallocMemory( void *old, size_t size )
{
    if ( size < 1 ) { size = 1; }
    char *p = perimeter_ptrcast( char *, std::realloc( old, size_t( size )));
    if ( p == nullptr ) {
        std::abort();
    }
    return p;
}

// ============================================================================
// free the memory
// ============================================================================
void    MemCntr :: freeMemory ( void *p )
{
    if ( p != nullptr ) { std::free( p ); }
}


#ifdef PERIMETER_QT5_ENABLED
// ////////////////////////////////////////////////////////////////////////////
//
//             nw: 20190523 1032 added, for QT5
//
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// memory count on QObject
// ============================================================================
QObject*  MemCntr :: memCntOnQObj (
    QObject *obj, const char *file, const char *func, int ln, const char *type
) {
    if ( obj != Q_NULLPTR ){
        MemCntr::incrNewCntr( 1, file, func, ln, type );
        QObject::connect(
            obj, & QObject::destroyed,
            [ file,func,ln,type ](QObject*) {
                MemCntr::decrNewCntr( 1, file, func, ln, type );
            }
        );
    } else {
        qFatal("the input object is null!");
    }
    return obj;
}

// ============================================================================
// memory count on destroy qobject
// ============================================================================
void      MemCntr :: memCntOnDelObj( QObject *o, bool is_later )
{
    if( o != Q_NULLPTR ) {
        if ( QThread::currentThread() == o->thread()) {
            if ( ! is_later ) { delete o; } else { o->deleteLater(); }
        } else {
            o->deleteLater();
        }
    } else {
        qFatal("the input object is null!");
    }
}

#endif


}



#ifdef __GNUC__ // in GCC 5, close below warnings
#pragma GCC diagnostic pop
#endif

#endif
