#ifndef QXPACK_IC_THRPIPE_CXX
#define QXPACK_IC_THRPIPE_CXX

#include "qxpack_ic_thrpipe.hxx"
#include "qxpack/indcom/common/qxpack_ic_memcntr.hxx"
#include "qxpack/indcom/common/qxpack_ic_pimplprivtemp.hpp"
#include "qxpack/indcom/common/qxpack_ic_objcache.hxx"
namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//              thr pipe package
//
// ////////////////////////////////////////////////////////////////////////////
#define T_PrivPkg( o ) T_ObjCast( IcThrPipePkgPriv*, o )
class QXPACK_IC_HIDDEN  IcThrPipePkgPriv : public QxPack::IcPImplPrivTemp<IcThrPipePkgPriv> {
private:
    int64_t    m_id;
    IcVariant  m_var;
public :
    explicit IcThrPipePkgPriv( );
    IcThrPipePkgPriv( const IcThrPipePkgPriv & );
    virtual ~IcThrPipePkgPriv( ) override;
    inline IcVariant &  varRef() { return m_var; }
    inline int64_t &    idRef()  { return m_id;  }
};

// ============================================================================
// ctor
// ============================================================================
IcThrPipePkgPriv :: IcThrPipePkgPriv ( )
{
}

// ============================================================================
// ctor ( copy )
// ============================================================================
IcThrPipePkgPriv :: IcThrPipePkgPriv ( const IcThrPipePkgPriv &ot )
{
    m_var = ot.m_var; m_id = ot.m_id;
}

// ============================================================================
// dtor
// ============================================================================
IcThrPipePkgPriv :: ~IcThrPipePkgPriv ( )
{
}

// ////////////////////////////////////////////////////////////////////////////
// wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
IcThrPipePkg :: IcThrPipePkg ( )
{  m_obj = nullptr; }

// ============================================================================
// dtor
// ============================================================================
IcThrPipePkg :: ~IcThrPipePkg ( )
{
    if ( m_obj != nullptr ) { IcThrPipePkgPriv::attach( &m_obj, nullptr ); }
}

// ============================================================================
// ctor ( copy )
// ============================================================================
IcThrPipePkg :: IcThrPipePkg ( const IcThrPipePkg &ot )
{
    m_obj = nullptr;
    IcThrPipePkgPriv::attach( &m_obj, const_cast<void**>(& ot.m_obj ));
}

// ============================================================================
// assign
// ============================================================================
IcThrPipePkg &   IcThrPipePkg :: operator = ( const IcThrPipePkg &ot )
{
    if ( this != &ot ) {
        IcThrPipePkgPriv::attach( &m_obj, const_cast<void**>(& ot.m_obj ) );
    }
    return *this;
}

#ifdef QXPACK_IC_CXX11
IcThrPipePkg :: IcThrPipePkg ( IcThrPipePkg && ot )
{  m_obj = ot.m_obj; ot.m_obj = nullptr; }

IcThrPipePkg &  IcThrPipePkg :: operator = ( IcThrPipePkg && ot )
{
    if ( m_obj != &ot ) {
        IcThrPipePkgPriv::attach( &m_obj, nullptr );
        m_obj = ot.m_obj;
    }
    return *this;
}

#endif

// ============================================================================
// access property
// ============================================================================
QxPack::IcVariant    IcThrPipePkg :: var ( ) const
{ return ( m_obj != nullptr ? T_PrivPkg(m_obj)->varRef() : IcVariant()); }

void    IcThrPipePkg :: setVar( const IcVariant &v )
{ IcThrPipePkgPriv::instanceCow( &m_obj )->varRef() = v; }

int64_t IcThrPipePkg :: id() const
{ return ( m_obj != nullptr ? T_PrivPkg(m_obj)->idRef() : 0 ); }

void    IcThrPipePkg :: setId( const int64_t &id )
{ IcThrPipePkgPriv::instanceCow( &m_obj )->idRef() = id; }


// ////////////////////////////////////////////////////////////////////////////
//
//  global pkg cache
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcThrPipePkgCache {
private:
    struct QXPACK_IC_HIDDEN OprIF : public IcObjCache::OprIF {
        virtual  ~OprIF() override { }
        virtual void    deleteSelf() override { qxpack_ic_delete( this, OprIF ); }
        virtual int     verID()  override { return 0; }  // the version ID used to identify interface version
        virtual void*   createObj () override { return qxpack_ic_new( IcThrPipePkg ); }
        virtual void    deinitObj ( void* ) override { }
        virtual void    initObj   ( void* ) override { }
        virtual void    deleteObj ( void *p ) override { qxpack_ic_delete( p, IcThrPipePkg ); }
    };

    IcObjCache  m_obj_cache;
public :
    explicit IcThrPipePkgCache ( );
    ~IcThrPipePkgCache ( );
};

// ============================================================================
//
// ============================================================================








// ////////////////////////////////////////////////////////////////////////////
//
//  thread pipe
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcThrPipePriv {
private:

public :

};



// ============================================================================
// ctor
// ============================================================================
IcThrPipe :: IcThrPipe ( )
{
    m_obj =
}



}

#endif
