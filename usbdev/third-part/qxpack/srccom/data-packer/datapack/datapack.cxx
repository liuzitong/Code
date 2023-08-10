#ifndef DATAPACK_CXX
#define DATAPACK_CXX

#include "datapack.hxx"
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <atomic> //C++11
#include <mutex>
#include <vector>
#include <cstdarg>
#include <cstdio>
#include <sys/stat.h> // this file included in linux and window

#if defined( _WIN32 ) || defined( __CYGWIN__ )
#  define PLAT_WIN32
#else
#  define PLAT_LINUX
#endif


namespace DataPack {

// ////////////////////////////////////////////////////////////////////////////
// [ mem cntr ] memory counter
// ////////////////////////////////////////////////////////////////////////////
static std::atomic<int>  g_atom(0);
static std::mutex        g_mem_trace_locker;
static std::atomic<MemCntrTrace>  g_mem_trace( nullptr );

static int   updateNewCntr ( int cntr, const char *file = nullptr, const char *func = nullptr, int ln = 0, const char *type = nullptr )
{
    auto old = g_atom.fetch_add(cntr);
    if ( g_mem_trace.load( std::memory_order_acquire ) != nullptr ) {
        g_mem_trace_locker.lock();
        auto ptr = g_mem_trace.load( std::memory_order_acquire );
        if ( ptr != nullptr ) { ptr( cntr, file, func, ln, type ); }
        g_mem_trace_locker.unlock();
    }
    return old + cntr;
}

DATAPACK_API  auto  setMemCntrTrace( MemCntrTrace tc ) -> MemCntrTrace
{
    g_mem_trace_locker.lock();
    auto old = g_mem_trace.load( std::memory_order_acquire );
    g_mem_trace.store( tc );
    g_mem_trace_locker.unlock();
    return old;
}

#if defined(_DEBUG)
#  define  datapack_new( t, ... )         ( DataPack::updateNewCntr( 1,__FILE__,__FUNCTION__,__LINE__, #t ),  new t ( __VA_ARGS__ ))
#  define  datapack_delete( o, t )      do{ DataPack::updateNewCntr(-1,__FILE__,__FUNCTION__,__LINE__, #t ); delete  static_cast< t *>( o ); }while(0)
#  define  datapack_alloc( sz )           ( DataPack::updateNewCntr( 1,__FILE__,__FUNCTION__,__LINE__ ),     std::malloc( sz ) )
#  define  datapack_realloc( p, sz )      ( DataPack::updateNewCntr(  ( p != nullptr ? 0 : 1 ), __FILE__,__FUNCTION__,__LINE__ ),  std::realloc( p, sz ) )
#  define  datapack_free( p )           do{ DataPack::updateNewCntr(-1,__FILE__,__FUNCTION__,__LINE__);  std::free( p ); }while(0)
#else
#  define  datapack_new( t, ... )         ( DataPack::updateNewCntr(1), new t ( __VA_ARGS__ ))
#  define  datapack_delete( o, t )      do{ DataPack::updateNewCntr(-1); delete static_cast< t *>( o ); }while(0)
#  define  datapack_alloc( sz )           ( DataPack::updateNewCntr(1), std::malloc( sz ) )
#  define  datapack_realloc( p, sz )      ( DataPack::updateNewCntr(( p != nullptr ? 0 : 1 )), std::realloc( p, sz ))
#  define  datapack_free( p )           do{ DataPack::updateNewCntr(-1); std::free( p ); }while(0)
#endif

#define PImplPrivTemp_PtrCast( t, o )  ( reinterpret_cast< t >( reinterpret_cast< intptr_t >( o )))
#define PImplPrivTemp_IntPtrCast( o )  ( reinterpret_cast< intptr_t >( o ))
#define PImplPrivTemp_new              datapack_new
#define PImplPrivTemp_delete           datapack_delete
#define datapack_objcast( t, o )       ( static_cast< t >( o ) )

// ////////////////////////////////////////////////////////////////////////////
// [ template ] PImpl. class
// ////////////////////////////////////////////////////////////////////////////
template <typename T>
class DATAPACK_HIDDEN PImplPrivTemp  {
public:
    PImplPrivTemp () { m_ref_cntr.store(1); }
    virtual ~PImplPrivTemp ( ) { }
    static T*    buildIfNull ( void **w_obj );
    static T*    createInstance ( )           { return PImplPrivTemp_new ( T ); }
    static T*    createInstance ( void *ref ) { return PImplPrivTemp_new ( T,  * PImplPrivTemp_PtrCast( T*, ref ) ); }
    static T*    addRef    ( void *cd )       {
        if ( cd != nullptr ) { PImplPrivTemp_PtrCast( T*, cd )->addRefCntr(); }
        return  PImplPrivTemp_PtrCast( T*, cd );
    }
    static bool  releaseRef( void *   );
    static bool  attach    ( void **src_obj, void **dst_obj );
    static T*    instanceCow ( void ** );
protected:
    inline int  addRefCntr( )    { return m_ref_cntr.fetch_add(1) + 1; }
    inline int  releaseRefCntr( ){ return m_ref_cntr.fetch_sub(1) - 1; }
    inline int  currRefCntr( )   { return m_ref_cntr.load(); }
private:
    std::atomic<int>  m_ref_cntr;
};

template <typename T >
auto     PImplPrivTemp<T> :: buildIfNull( void **w_obj ) -> T*
{
    if (  w_obj == nullptr ) { return nullptr; }
    if ( *w_obj == nullptr ) { *w_obj = PImplPrivTemp<T>::createInstance( ); }
    return PImplPrivTemp_PtrCast( T*, *w_obj );
}

template <typename T>
auto     PImplPrivTemp<T> :: releaseRef ( void *cd ) -> bool
{
    bool is_released = false;
    if ( cd != nullptr ) {
        if ( PImplPrivTemp_PtrCast( T*, cd )->releaseRefCntr() == 0 ) {
            PImplPrivTemp_delete( cd, T );
            is_released = true;
        }
    }
    return is_released;
}

template <typename T >
auto     PImplPrivTemp<T> :: attach ( void **src_obj, void **dst_obj ) -> bool
{
    if ( src_obj == dst_obj ) { return true; }
    if ( src_obj == nullptr ) { return false; }
    if ( dst_obj != nullptr  &&  *src_obj == *dst_obj ) { return true; } // need not to attach.

    if ( *src_obj != nullptr ) {
        PImplPrivTemp<T>::releaseRef( *src_obj );
        *src_obj = nullptr;
    }

    intptr_t tmp_ptr = 0x100;
    if ( dst_obj > reinterpret_cast<void**>( tmp_ptr )) {
        if ( *dst_obj != nullptr ) {
            *src_obj = PImplPrivTemp<T>::addRef( *dst_obj );
        }
    }

    return true;
}

template <typename T>
auto     PImplPrivTemp<T> :: instanceCow ( void **w_obj ) -> T*
{
    if (  w_obj == nullptr ) { return nullptr; }
    if ( *w_obj == nullptr ) { *w_obj = PImplPrivTemp<T>::createInstance( ); }
    if ( PImplPrivTemp_PtrCast( T*, *w_obj)->currRefCntr( ) == 1 ) {
        return PImplPrivTemp_PtrCast( T*, *w_obj );
    }

    T *new_obj = PImplPrivTemp<T>::createInstance( *w_obj );
    PImplPrivTemp<T>::releaseRef( *w_obj );
    *w_obj = new_obj;

    return new_obj;
}

// ////////////////////////////////////////////////////////////////////////////
// [ bytearray ]
// ////////////////////////////////////////////////////////////////////////////
#define T_Ba( o )  datapack_objcast( ByteArrayPriv*, o )
class DATAPACK_HIDDEN  ByteArrayPriv : public PImplPrivTemp<ByteArrayPriv> {
private:
    void *m_data; size_t m_size;
public :
    explicit ByteArrayPriv( ) : m_data( nullptr ), m_size( 0 ) { }
    virtual ~ByteArrayPriv( ) DATAPACK_OVERRIDE;
    ByteArrayPriv( const ByteArrayPriv &ot );
    auto  init( size_t sz ) -> void;
    inline auto dataPtr() const -> void*   { return m_data; }
    inline auto size()    const -> size_t  { return m_size; }
};

ByteArrayPriv :: ~ByteArrayPriv ( )
{ if ( m_data != nullptr ) { datapack_free( m_data ); }}

ByteArrayPriv :: ByteArrayPriv ( const ByteArrayPriv &ot )
{
    m_data = nullptr; m_size = 0;
    if ( ot.m_data != nullptr && ot.m_size > 0 ) {
        m_data = datapack_alloc( ot.m_size );
        std::memcpy( m_data, ot.m_data, ot.m_size );
        m_size = ot.m_size;
    }
}

auto     ByteArrayPriv :: init ( size_t sz  ) -> void
{
    if ( sz < 1 ) { sz = 1; }
    m_data = datapack_alloc( sz );
    m_size = sz;
    std::memset( m_data, 0, m_size );
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
ByteArray ::  ByteArray ( ) { m_obj = nullptr; }
ByteArray :: ~ByteArray ( ) { makeNull(); }

ByteArray :: ByteArray ( size_t size )
{  m_obj = nullptr; ByteArrayPriv::buildIfNull( &m_obj )->init( size ); }

ByteArray :: ByteArray ( const ByteArray &ot )
{  m_obj = nullptr; ByteArrayPriv::attach( &m_obj, const_cast<void**>( &ot.m_obj )); }

auto     ByteArray :: operator = ( const ByteArray &ot ) -> ByteArray &
{  ByteArrayPriv::attach( &m_obj, const_cast<void**>(& ot.m_obj )); return *this; }

auto     ByteArray :: makeNull() -> void
{ if ( m_obj != nullptr ) { ByteArrayPriv::attach( &m_obj, nullptr ); m_obj = nullptr; }}

auto     ByteArray :: isNull()    const -> bool   { return m_obj == nullptr; }
auto     ByteArray :: size()      const -> size_t { return ( m_obj != nullptr ? T_Ba(m_obj)->size() : 0 ); }
auto     ByteArray :: constData() const -> const void* { return ( m_obj != nullptr ? T_Ba(m_obj)->dataPtr() : nullptr ); }

// ////////////////////////////////////////////////////////////////////////////
// [ dir ent ]
// ////////////////////////////////////////////////////////////////////////////
struct DATAPACK_HIDDEN DirEntItem {
    int64_t rec_ofv; int64_t rec_size;
    DirEntItem ( ) : rec_ofv(0), rec_size(0) { }
    DirEntItem ( const int64_t &ofv, const int64_t &sz ) : rec_ofv(ofv), rec_size(sz) { }
};

// ////////////////////////////////////////////////////////////////////////////
// [ header ]
// ////////////////////////////////////////////////////////////////////////////
#define HDR_VER  0x015a
#define DEFAULT_DIR_ENT_NUM  16

struct DATAPACK_HIDDEN HeaderV0 {
    uint16_t  check_sum_inv, version;       //  invert the check sum value, and current header structure version
    uint16_t  hdr_sz,  rsvd;                //
    uint32_t  dir_ent_num, dir_ent_used;    //
    int64_t   total_sz, rsvd2;              // current file size

    HeaderV0 ( )
    { init( DEFAULT_DIR_ENT_NUM ); }

    auto  init( uint32_t  dir_ent ) -> void
    {
        version      = HDR_VER;
        hdr_sz       = sizeof( HeaderV0 );  rsvd = 0;
        dir_ent_num  = ( dir_ent < 1 ? 1 : dir_ent );
        dir_ent_used = 0;
        total_sz     = sizeof( HeaderV0 ) + dirEntSize();
        rsvd2        = 0;

        updateCheckSum();
    }

    inline auto  updateCheckSum() -> void
    {  check_sum_inv = ~calcCheckSum(); }

    auto  calcCheckSum() const -> uint16_t
    {
        auto ptr = reinterpret_cast<const uint8_t*>(this);
        auto rsl = uint16_t( 0 );
        for ( size_t i = sizeof(uint16_t); i < sizeof(HeaderV0); i ++ ) { rsl += ptr[i]; }
        return rsl;
    }

    auto  validateHdr() const -> bool
    { return hdr_sz == sizeof(HeaderV0) && uint16_t( calcCheckSum()) == uint16_t( ~check_sum_inv ); }

    inline auto  recNum()     const -> size_t { return dir_ent_used; }
    inline auto  dirEntSize() const -> size_t { return dir_ent_num * sizeof(DirEntItem); }
};

#define T_Hdr( o )  datapack_objcast( HeaderPriv*, o )
class DATAPACK_HIDDEN  HeaderPriv : public PImplPrivTemp< HeaderPriv > {
private:
    HeaderV0  m_hdr_v0;
public :
    explicit HeaderPriv ( );
    HeaderPriv ( const HeaderPriv & );
    virtual ~HeaderPriv( ) DATAPACK_OVERRIDE;
    inline auto  headerV0Ref() -> HeaderV0& { return m_hdr_v0; }
};

HeaderPriv :: HeaderPriv ( )
{
}

HeaderPriv :: HeaderPriv ( const HeaderPriv &ot )
{   m_hdr_v0 = ot.m_hdr_v0;  }

HeaderPriv :: ~HeaderPriv ( ) { }

Header ::  Header ( ) { m_obj = nullptr; }
Header :: ~Header ( ) { makeNull(); }
Header ::  Header ( const Header &ot ) { m_obj = nullptr; HeaderPriv::attach( &m_obj, const_cast<void**>(&ot.m_obj)); }
auto     Header :: operator = ( const Header &ot ) -> Header & { HeaderPriv::attach(&m_obj, const_cast<void**>(&ot.m_obj)); return *this; }
auto     Header :: isNull() const -> bool { return m_obj == nullptr; }
auto     Header :: makeNull() -> void { if ( m_obj != nullptr ){ HeaderPriv::attach(&m_obj, nullptr); m_obj = nullptr; }}
auto     Header :: version() const -> uint16_t { return ( m_obj != nullptr ? T_Hdr(m_obj)->headerV0Ref().version : 0 ); }
auto     Header :: totalRecNum()  const -> uint32_t { return ( m_obj != nullptr ? T_Hdr(m_obj)->headerV0Ref().dir_ent_num  : 0 ); }
auto     Header :: usedRecNum()   const -> uint32_t { return ( m_obj != nullptr ? T_Hdr(m_obj)->headerV0Ref().dir_ent_used : 0 ); }


// ////////////////////////////////////////////////////////////////////////////
// [ DevIo ]
// ////////////////////////////////////////////////////////////////////////////
DevIo ::  DevIo ( ) { m_obj = nullptr; }
DevIo :: ~DevIo ( ) { }

// ////////////////////////////////////////////////////////////////////////////
// [ file io ]
// ////////////////////////////////////////////////////////////////////////////
#if  defined( PLAT_WIN32 )
typedef struct __stat64  FileIo_Stat;
#else
typedef struct stat64    FileIo_Stat;
#endif

#define T_Fio( o ) datapack_objcast( FileIoPriv*, o )
class DATAPACK_HIDDEN  FileIoPriv {
private:
    FILE *m_fp; int m_fno; FileIo::Mode m_mode;
    FileIo_Stat  m_fst;
public :
    explicit FileIoPriv( const char *fp, FileIo::Mode mode );
    ~FileIoPriv( );
    inline auto  filePtrRef()-> FILE*&   { return m_fp;   }
    inline auto  modeRef()   -> FileIo::Mode& { return m_mode; }
    inline auto  fstRef()    -> FileIo_Stat&  { return m_fst; }
    inline auto  fileNoRef() -> int&     { return m_fno; }
};

#if  defined( PLAT_WIN32 )
FileIoPriv :: FileIoPriv ( const char *fp, FileIo::Mode mode )
{
    m_mode = mode;
    m_fp = nullptr; errno_t err = 0;
    switch( mode ) {
    case FileIo::Mode_ReadOnly : err = fopen_s( &m_fp, fp, "rb" ); break;
    case FileIo::Mode_WriteOnly: err = fopen_s( &m_fp, fp, "wb" ); break; // will clear content
    default:
        err = fopen_s( &m_fp, fp, "r+b" ); // the file is existed ?
        if ( err != 0 ) { err = fopen_s( &m_fp, fp, "w+b"); } // create new file for RW
    }
    if ( err == 0 ) {
        m_fno = _fileno( m_fp );
    } else {
        m_fp = nullptr; m_fno = -1;
    }
}

#else // for posix
FileIoPriv :: FileIoPriv ( const char *fp, FileIo::Mode mode )
{
    m_mode = mode;
    m_fp = nullptr;
    switch( mode ) {
    case FileIo::Mode_ReadOnly : m_fp = fopen64( fp, "r" ); break;
    case FileIo::Mode_WriteOnly: m_fp = fopen64( fp, "w" ); break;
    default:
        m_fp = fopen64( fp, "r+");
        if ( m_fp == nullptr ) { m_fp = fopen64( fp, "w+"); }
    }
    if ( m_fp != nullptr ) {
        m_fno = fileno( m_fp );
    } else {
        m_fno = -1;
    }
}
#endif


FileIoPriv :: ~FileIoPriv ( )
{  if ( m_fp != nullptr ) { std::fflush( m_fp ); std::fclose( m_fp ); } }

FileIo :: FileIo ( const char *fp, Mode mode )
{ m_obj = datapack_new( FileIoPriv, fp, mode ); }

FileIo :: ~FileIo ( )
{ datapack_delete( m_obj, FileIoPriv ); }

auto     FileIo :: isVaild() const -> bool
{ return T_Fio(m_obj)->filePtrRef() != nullptr ; }

auto     FileIo :: size() const -> int64_t
{
    auto priv = T_Fio( m_obj );
    if ( priv->filePtrRef() == nullptr ) { return -1; }
#if defined( PLAT_WIN32 )
    auto ret = _fstat64( priv->fileNoRef(), & priv->fstRef() );
#else
    auto ret = fstat64( priv->fileNoRef(), & priv->fstRef() );
#endif
    return ( ret == 0 ? priv->fstRef().st_size  : -1 );
}

auto     FileIo :: pos() const -> int64_t
{
    auto priv = T_Fio( m_obj );
    if ( priv->filePtrRef() == nullptr ) { return -1; }
#if defined( PLAT_WIN32 )
    return _ftelli64( priv->filePtrRef() );
#else
    return ftello64( priv->filePtrRef() );
#endif
}

auto     FileIo :: seek( int64_t pos ) -> bool
{
    auto priv = T_Fio( m_obj );
    if ( priv->filePtrRef() == nullptr ) { return false; }
    if ( pos < 0 ) { pos = 0; }
#if defined( PLAT_WIN32 )
    return _fseeki64( priv->filePtrRef(), pos, SEEK_SET ) == 0;
#else
    return fseeko64( priv->filePtrRef(), pos, SEEK_SET ) == 0;
#endif
}

auto     FileIo :: read( void *dp, size_t sz ) -> size_t
{
    auto priv = T_Fio( m_obj );
    if ( priv->filePtrRef() == nullptr ) { return 0; }
    if ( sz < 1 ) { return 0; }
#if defined( PLAT_WIN32 )
# if  !defined(__CYGWIN__)
    return fread_s( dp, sz, 1, sz, priv->filePtrRef() );
# else
    return fread( dp, 1, sz, priv.filePtrRef());
# endif
#else
    return fread( dp, 1, sz, priv->filePtrRef());
#endif
}

auto     FileIo :: write( const void *dp, size_t sz ) -> size_t
{
    auto priv = T_Fio(m_obj);
    if ( priv->filePtrRef() == nullptr ) { return 0; }
    if ( sz < 1 ) { return 0; }
    return fwrite( dp, 1, sz, priv->filePtrRef() );
}


// ////////////////////////////////////////////////////////////////////////////
// [ memory io ]
// ////////////////////////////////////////////////////////////////////////////
#define T_Mio( o )  datapack_objcast( MemIoPriv*, o )
class DATAPACK_HIDDEN  MemIoPriv {
private:
    int64_t m_pos;
    std::vector<uint8_t>  m_incr_buff;
    void  *m_spec_buff;  size_t m_spec_sz;
    MemIo::Cleanup  m_cl_up;  void  *m_cl_up_ctxt;
public :
    explicit MemIoPriv( );
    MemIoPriv ( void *mem, size_t mem_sz, MemIo::Cleanup cl_up, void *cl_up_ctxt );
    ~MemIoPriv( );
    inline auto  incrBuffRef() -> std::vector<uint8_t> &  { return m_incr_buff; }
    inline auto  specBuffRef() -> void*& { return m_spec_buff; }
    inline auto  specBuffSize()-> size_t { return m_spec_sz; }
    inline auto  dataPtr()  -> void*    { return ( m_spec_buff != nullptr ? m_spec_buff : m_incr_buff.data() ); }
    inline auto  dataSize() -> size_t   { return ( m_spec_buff != nullptr ? m_spec_sz   : m_incr_buff.size() ); }
    inline auto  posRef()   -> int64_t& { return m_pos;  }
};

MemIoPriv :: MemIoPriv ( )
{
    m_spec_buff = nullptr; m_spec_sz = 0;
    m_cl_up     = nullptr; m_cl_up_ctxt = nullptr;
    m_pos = 0;
}

MemIoPriv :: MemIoPriv ( void *mem, size_t mem_sz, MemIo::Cleanup cl_up, void *cl_up_ctxt )
{
    m_spec_buff = mem;    m_spec_sz = mem_sz;
    m_cl_up     = cl_up;  m_cl_up_ctxt = cl_up_ctxt;
    m_pos       = 0;
}

MemIoPriv :: ~MemIoPriv ( )
{
    if ( m_cl_up != nullptr && m_spec_buff != nullptr ) {
        m_cl_up( m_spec_buff, m_cl_up_ctxt );
    }
}

MemIo :: MemIo ( )
{
    m_obj = datapack_new( MemIoPriv );
}

MemIo :: ~MemIo ( )
{
    datapack_delete( m_obj, MemIoPriv );
}

auto     MemIo :: data() const -> const void*
{ return T_Mio(m_obj)->dataPtr(); }

auto     MemIo :: size() const -> int64_t
{ return T_Mio(m_obj)->dataSize(); }

auto     MemIo :: pos() const -> int64_t
{ return int64_t( T_Mio(m_obj)->posRef()); }

auto     MemIo :: seek( int64_t pos ) -> bool
{
    auto priv = T_Mio(m_obj);

    if ( priv->specBuffRef() != nullptr ) {
        // fixed size memory, 'pos' cannot point beyond the maximum value
        auto sz = int64_t( priv->specBuffSize());
        if ( pos < sz ) {
            priv->posRef() = pos;
            return true;
        } else {
            return false;
        }
    } else {
        // auto increase memory
        auto sz = int64_t( priv->incrBuffRef().size());
        if ( pos > sz ) { priv->incrBuffRef().resize( size_t( pos )); }
        priv->posRef() = pos;
        return true;
    }
}

auto     MemIo :: read( void *dp, size_t sz ) -> size_t
{
    if ( dp == nullptr || sz < 1  ) { return 0; }

    auto priv = T_Mio(m_obj);

    // read operation could not over the size
    auto src    = priv->dataPtr();
    auto src_sz = priv->dataSize();
    auto rem_sz = int64_t( src_sz ) - priv->posRef(); // maybe 0.
    auto cpy_sz = size_t( rem_sz <= int64_t(sz) ? rem_sz : sz );

    std::memcpy( dp, reinterpret_cast<uint8_t*>( src ) + priv->posRef(), cpy_sz );
    priv->posRef() += cpy_sz;

    return cpy_sz;
}

auto     MemIo :: write( const void *dp, size_t sz ) -> size_t
{
    if ( dp == nullptr || sz < 1 ) { return 0; }

    auto priv = T_Mio(m_obj);
    if ( priv->specBuffRef() != nullptr ) {
        // fixed memory, could not write out side
        auto rem_sz = int64_t( priv->specBuffSize()) - priv->posRef();
        auto cpy_sz = size_t( rem_sz <= int64_t(sz) ? rem_sz : sz );
        std::memcpy( reinterpret_cast<uint8_t*>(priv->specBuffRef()) + size_t( priv->posRef()), dp, cpy_sz );
        priv->posRef() += cpy_sz;

        return cpy_sz;
    } else {
        // auto increase memory, the write operation maybe increase the memory
        auto vec    = priv->incrBuffRef();
        auto exp_sz = priv->posRef() + sz;
        if ( exp_sz > int64_t( vec.size()) ) { vec.resize( size_t( exp_sz )); }
        std::memcpy( vec.data(), dp, sz );
        priv->posRef() += sz;

        return sz;
    }
}




// ////////////////////////////////////////////////////////////////////////////
// [ writer ]
// ////////////////////////////////////////////////////////////////////////////
#define T_Wrt( o ) datapack_objcast( WriterPriv*, o )
class DATAPACK_HIDDEN WriterPriv {
private:
    HeaderV0     m_hdr_v0;
    DirEntItem  *m_dir_ent;
    DevIo       *m_dev_io;
    bool         m_is_init_ok, m_is_need_flush;
public :
    explicit WriterPriv( );
    ~WriterPriv( );
    auto init( DevIo*, uint32_t dir_ent ) -> void;
    inline auto  markNeedFlush() -> void     { m_is_need_flush = true; }
    inline auto  devIoRef()   -> DevIo*&     { return m_dev_io; }
    inline auto  isInitOK()   -> bool        { return m_is_init_ok; }
    inline auto  dirEntPtr()  -> DirEntItem* { return m_dir_ent;  }
    inline auto  headerV0Ref()-> HeaderV0&   { return m_hdr_v0; }
    auto  flush() -> void;
};

WriterPriv :: WriterPriv( )
{
    m_dev_io  = nullptr;  m_dir_ent = nullptr;
    m_is_init_ok = false; m_is_need_flush = true;
}

WriterPriv :: ~WriterPriv( )
{
    flush();
    if ( m_dir_ent != nullptr ) { datapack_free( m_dir_ent ); }
}

auto     WriterPriv :: flush() -> void
{
    if ( m_is_init_ok && m_is_need_flush ) {
        m_hdr_v0.updateCheckSum();
        auto ret = m_dev_io->seek( 0 ); auto hdr_sz = sizeof(m_hdr_v0);
        if ( ret ) { ret = ( m_dev_io->write( &m_hdr_v0, hdr_sz ) == hdr_sz ); }
        if ( ret ) { ret = m_dev_io->seek( sizeof(HeaderV0) ); }
        if ( ret ) { ret = ( m_dev_io->write( m_dir_ent, m_hdr_v0.dirEntSize() ) == m_hdr_v0.dirEntSize()); }
        if ( ret ) { m_is_need_flush = false; }
    }
}

auto     WriterPriv :: init( DevIo *dev_io, uint32_t dir_ent ) -> void
{
    if ( ( m_dev_io = dev_io ) == nullptr ) { return; }

    // try read the header
    auto ret = true; auto is_need_ctor_hdr = false; auto hdr_sz = sizeof(m_hdr_v0);
    if ( m_dev_io->size() >= sizeof(HeaderV0)) {
        ret = m_dev_io->seek( 0 );
        if ( ret ) { ret = ( m_dev_io->read( & m_hdr_v0, hdr_sz ) == hdr_sz ); }
        if ( ret ) { ret = m_hdr_v0.validateHdr(); }
        if ( ret ) { m_is_need_flush = false; } else { is_need_ctor_hdr = true; }
    } else {
        is_need_ctor_hdr = true;
    }

    // rebuild header for this data packing
    if ( is_need_ctor_hdr ) {
        m_hdr_v0.init( dir_ent ); // no table entrys
        ret = m_dev_io->seek(0);
        if ( ret ) { // write header
            ret = ( m_dev_io->write( & m_hdr_v0, hdr_sz ) == hdr_sz );
        }
        if ( ret ) { // write dir entry
            ret = m_dev_io->seek( sizeof(HeaderV0) );
            m_dir_ent = reinterpret_cast<DirEntItem*>( datapack_alloc( m_hdr_v0.dirEntSize() ));
            std::memset( m_dir_ent, 0, m_hdr_v0.dirEntSize() );
            if ( ret ) { ret = ( m_dev_io->write( m_dir_ent, m_hdr_v0.dirEntSize() ) == m_hdr_v0.dirEntSize() ); }
            if (!ret ) { datapack_free( m_dir_ent ); m_dir_ent = nullptr; }
        }
        if ( ret ) { m_is_need_flush = false; }
    } else {
        // reader header ok, then read directory mem.
        ret = m_dev_io->seek( sizeof(HeaderV0));
        if ( ret ) {
            m_dir_ent = reinterpret_cast<DirEntItem*>( datapack_alloc( m_hdr_v0.dirEntSize()));
            ret = ( m_dev_io->read( m_dir_ent, m_hdr_v0.dirEntSize() ) == m_hdr_v0.dirEntSize() );
        }
        if (!ret ) { datapack_free( m_dir_ent ); m_dir_ent = nullptr; }
    }

    m_is_init_ok = ret;
}

Writer :: Writer ( DevIo *io, uint32_t dir_ent_num)
{
    m_obj = datapack_new( WriterPriv );
    auto priv = T_Wrt( m_obj );
    priv->init( io, dir_ent_num > 0 ? dir_ent_num  : 1 );
}

Writer :: ~Writer( )
{
    datapack_delete( m_obj, WriterPriv );
}

auto     Writer :: isValid() const -> bool
{ return  T_Wrt(m_obj)->isInitOK(); }

auto     Writer :: flush() -> void
{  T_Wrt(m_obj)->flush();  }

auto     Writer :: writeRecord( const void *dp, size_t sz, uint32_t *out_idx ) -> bool
{
    auto priv = T_Wrt( m_obj );
    if ( ! priv->isInitOK() || dp == nullptr ) { return false; }
    if ( int64_t(sz) < 0 ) { return false; }

    auto &hdr = priv->headerV0Ref();
    auto &dev = priv->devIoRef();
    auto  dir = priv->dirEntPtr();
    if ( hdr.dir_ent_used >= hdr.dir_ent_num ) { return false; }

    auto ofv = hdr.total_sz;
    auto idx = hdr.dir_ent_used;
    auto ret = dev->seek( ofv );
    if ( ret ) { ret = ( dev->write( dp, sz ) == sz ); }
    if ( ret ) {
        hdr.total_sz += int64_t( sz );
        dir[ hdr.dir_ent_used ].rec_ofv  = ofv;
        dir[ hdr.dir_ent_used ].rec_size = sz;
        ++ hdr.dir_ent_used;
        if ( out_idx != nullptr ) { *out_idx = idx; }
     }
     if ( ret ) { priv->markNeedFlush(); }
     return ret;
}

auto     Writer :: writeRecord(const ByteArray &ba, uint32_t *out_idx ) -> bool
{ return writeRecord( ba.constData(), ba.size(), out_idx ); }



// ////////////////////////////////////////////////////////////////////////////
// [ reader ]
// ////////////////////////////////////////////////////////////////////////////
#define T_Rdr( o ) datapack_objcast( ReaderPriv*, o )
class DATAPACK_HIDDEN  ReaderPriv {
private:
    HeaderV0     m_hdr_v0;
    DirEntItem  *m_dir_ent;
    DevIo       *m_dev_io;
    bool         m_is_init_ok;
public:
    explicit ReaderPriv();
    ~ReaderPriv();
    auto  init( DevIo* ) -> void;
    inline auto  isInitOK() const -> bool    { return m_is_init_ok; }
    inline auto  dirEntPtr()  -> DirEntItem* { return m_dir_ent;  }
    inline auto  headerV0Ref()->HeaderV0&    { return m_hdr_v0; }
    inline auto  devIoRef()   -> DevIo*&     { return m_dev_io; }
};

ReaderPriv :: ReaderPriv ( )
{
    m_dir_ent = nullptr; m_dev_io = nullptr; m_is_init_ok = false;
}

ReaderPriv :: ~ReaderPriv ( )
{
    if ( m_dir_ent != nullptr ) { datapack_free( m_dir_ent ); }
}

auto     ReaderPriv :: init( DevIo *dev_io ) -> void
{
    if ( ( m_dev_io = dev_io ) == nullptr ) { return; }

    auto ret = true; auto hdr_sz = sizeof(m_hdr_v0);
    if ( m_dev_io->size() >= sizeof(HeaderV0)) {
        ret = m_dev_io->seek( 0 );
        if ( ret ) { ret = ( m_dev_io->read( & m_hdr_v0, hdr_sz ) == hdr_sz ); }
        if ( ret ) { ret = m_hdr_v0.validateHdr(); }
        if ( ret ) {
            m_dir_ent = reinterpret_cast<DirEntItem*>( datapack_alloc( m_hdr_v0.dirEntSize()));
        }
        if ( ret ) { m_dev_io->seek( sizeof(HeaderV0) ); }
        if ( ret && m_dir_ent != nullptr ) {
            ret = ( m_dev_io->read( m_dir_ent, m_hdr_v0.dirEntSize() ) == m_hdr_v0.dirEntSize() );
        }
        if ( ! ret && m_dir_ent != nullptr ) { datapack_free( m_dir_ent ); m_dir_ent = nullptr; }
        m_is_init_ok = ret;
    }
}

Reader :: Reader ( DevIo *io )
{
    m_obj = datapack_new( ReaderPriv );
    auto priv = T_Rdr( m_obj );
    priv->init( io );
}

Reader :: ~Reader()
{
    datapack_delete( m_obj, ReaderPriv );
}

auto     Reader :: isValid() const -> bool { return T_Rdr(m_obj)->isInitOK(); }

auto     Reader :: header() const -> Header
{
    auto priv = T_Rdr(m_obj);
    if ( ! priv->isInitOK()) { return Header(); }

    Header hdr;
    HeaderPriv::buildIfNull( &hdr.m_obj );
    T_Hdr(hdr.m_obj)->headerV0Ref() = priv->headerV0Ref();
    return hdr;
}

auto     Reader :: readRecord( uint32_t idx ) -> ByteArray
{
    auto priv = T_Rdr(m_obj);
    if ( ! priv->isInitOK()) { return ByteArray(); }

    auto &hdr = priv->headerV0Ref();
    auto &dev = priv->devIoRef();
    auto  dir = priv->dirEntPtr();
    if ( idx >= hdr.dir_ent_num ) { return ByteArray(); } // over maximum number

    auto ofv = dir[ idx ].rec_ofv;
    auto sz  = dir[ idx ].rec_size;
    if ( sz > size_t( -1L )) { return ByteArray(); }

    auto ret = dev->seek( ofv );
    if ( ret ) {
        ByteArray rsl( (size_t( sz )) );
        ret = ( dev->read( const_cast<void*>( rsl.constData()), size_t( sz )) == sz );
        if ( ret ) { return rsl; } else { return ByteArray(); }
    } else {
        return ByteArray();
    }
}


}
#endif
