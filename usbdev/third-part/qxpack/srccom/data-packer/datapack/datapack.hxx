// ////////////////////////////////////////////////////////////////////////////
// File  : data packing utils
// Descr.: this utils used to packing data as many records to one memory block
// Author: W.B.
// Update:
//  20210608 -- init version.
//  20210625 -- chg read/write return value, add Memory Device IO
// ////////////////////////////////////////////////////////////////////////////
#ifndef DATAPACK_HXX
#define DATAPACK_HXX

#include <cstdint>
#include <cstdlib>

namespace DataPack {

// ////////////////////////////////////////////////////////////////////////////
// definitions
// ////////////////////////////////////////////////////////////////////////////
#define DATAPACK_API
#define DATAPACK_HIDDEN

typedef void  (* MemCntrTrace  )( int req_cntr, const char *file, const char *func, int ln, const char *type );


#if __cplusplus >= 201103L || ( defined(_MSC_VER) && _MSC_VER >= 1800 )
#  define DATAPACK_CXX11
#  define DATAPACK_FINAL    final
#  define DATAPACK_OVERRIDE override
#else
#  undef  DATAPACK_CXX11
#  define DATAPACK_FINAL
#  define DATAPACK_OVERRIDE
#  error "this source need c++11 !"
#endif

#define DATAPACK_DISABLE_COPY( t ) private : t ( const t & ); t & operator = ( const t & );

DATAPACK_API  MemCntrTrace  setMemCntrTrace( MemCntrTrace );

// ////////////////////////////////////////////////////////////////////////////
// data bytes
// ////////////////////////////////////////////////////////////////////////////
class DATAPACK_API  ByteArray DATAPACK_FINAL {
public:
    explicit ByteArray();
    ByteArray ( size_t size );
    ~ByteArray();
    ByteArray ( const ByteArray & );
    ByteArray & operator = ( const ByteArray & );
#ifdef DATAPACK_CXX11
    ByteArray ( ByteArray &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    ByteArray & operator = ( ByteArray &&ot ) { if ( m_obj != ot.m_obj ) { makeNull(); m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif
    void         makeNull();
    bool         isNull() const;
    size_t       size() const;
    const void*  constData() const;
private:
    void *m_obj;
};

// ////////////////////////////////////////////////////////////////////////////
// header of the packed data
// ////////////////////////////////////////////////////////////////////////////
class DATAPACK_API  Reader;
class DATAPACK_API  Header DATAPACK_FINAL {
public:
    explicit Header();
    Header ( const Header & );
    Header & operator = ( const Header & );
    ~Header();
#ifdef DATAPACK_CXX11
    Header ( Header &&ot ) { m_obj = ot.m_obj; ot.m_obj = nullptr; }
    Header & operator = ( Header &&ot ) { if ( m_obj != ot.m_obj ) { makeNull(); m_obj = ot.m_obj; ot.m_obj = nullptr; } return *this; }
#endif
    bool      isNull() const;
    void      makeNull();
    uint16_t  version() const;
    uint32_t  totalRecNum() const;
    uint32_t  usedRecNum()  const;
private:
    void *m_obj;
    friend class Reader;
};

// ////////////////////////////////////////////////////////////////////////////
//  IO for writer and reader
// ////////////////////////////////////////////////////////////////////////////
class DATAPACK_API  DevIo {
public:
    explicit DevIo();
    virtual ~DevIo();
    virtual int64_t   size () const = 0;
    virtual int64_t   pos  () const = 0;
    virtual bool      seek ( int64_t pos ) = 0;
    virtual size_t    read (       void *dp, size_t sz ) = 0;
    virtual size_t    write( const void *dp, size_t sz ) = 0;
private:
    void *m_obj;
    DATAPACK_DISABLE_COPY( DevIo )
};

class DATAPACK_API  FileIo : public DevIo {
public:
    enum  Mode {
        Mode_ReadOnly = 0,  // file existed, read OK, otherwise isValid() return false
        Mode_WriteOnly,     // clear content or create new file
        Mode_ReadWrite      // try open existed file for read write, if not existed, create new one
    };
    explicit FileIo( const char *file_path, Mode mode );
    bool    isVaild() const;
    virtual ~FileIo() DATAPACK_OVERRIDE;
    virtual int64_t   size() const DATAPACK_OVERRIDE;
    virtual int64_t   pos()  const DATAPACK_OVERRIDE;
    virtual bool      seek( int64_t pos ) DATAPACK_OVERRIDE;
    virtual size_t    read( void *dp, size_t sz ) DATAPACK_OVERRIDE;
    virtual size_t    write( const void *dp, size_t sz ) DATAPACK_OVERRIDE;
private:
    void *m_obj;
    DATAPACK_DISABLE_COPY( FileIo )
};

class DATAPACK_API  MemIo : public DevIo {
public :
    typedef void (* Cleanup )( void *mem_obj, void *ctxt );

    explicit MemIo ( );  // auto-increase memory for write

    //! @note  read-write spec. memory. the memory size is fixed
    MemIo ( void *mem, size_t mem_sz, Cleanup cl_up = nullptr, void *cl_up_ctxt = nullptr );

    virtual ~MemIo ( ) DATAPACK_OVERRIDE;

    //! @note  call this function to get new data pointer after write or seek!
    const void*  data() const;

    virtual int64_t  size() const DATAPACK_OVERRIDE;
    virtual int64_t  pos()  const DATAPACK_OVERRIDE;
    virtual bool     seek( int64_t pos ) DATAPACK_OVERRIDE;
    virtual size_t   read( void *dp, size_t sz ) DATAPACK_OVERRIDE;
    virtual size_t   write( const void *dp, size_t sz ) DATAPACK_OVERRIDE;
private:
    void *m_obj;
    DATAPACK_DISABLE_COPY( MemIo )
};

// ////////////////////////////////////////////////////////////////////////////
// packing the data
// ////////////////////////////////////////////////////////////////////////////
class DATAPACK_API  Writer DATAPACK_FINAL {
public:
    explicit Writer( DevIo *io, uint32_t dir_ent_num );
    ~Writer();
    bool  isValid() const;
    bool  writeRecord( const ByteArray &, uint32_t *idx = nullptr );
    bool  writeRecord( const void *dp, size_t  sz, uint32_t *idx = nullptr );
    void  flush();
private:
    void *m_obj;
    DATAPACK_DISABLE_COPY( Writer )
};

// ////////////////////////////////////////////////////////////////////////////
// read the data
// ////////////////////////////////////////////////////////////////////////////
class DATAPACK_API  Reader DATAPACK_FINAL {
public:
    explicit Reader ( DevIo *io );
    ~Reader();
    bool        isValid() const;
    Header      header() const;
    ByteArray   readRecord( uint32_t idx );
private:
    void *m_obj;
    DATAPACK_DISABLE_COPY( Reader )
};

}

#endif
