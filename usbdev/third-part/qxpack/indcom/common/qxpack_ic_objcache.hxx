#ifndef QXPACK_IC_OBJCACHETEMP_HXX
#define QXPACK_IC_OBJCACHETEMP_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                      Object Cache [ Thread Safe ]
//
// ////////////////////////////////////////////////////////////////////////////
class  QXPACK_IC_API  IcObjCache QXPACK_IC_FINAL {
public :
    typedef void*   ObjPtr;
    // ========================================================================
    // the operation interface
    // : the object has below states:
    //  1) create   --  object has been created for using ( already init )
    //  2) deinit   --  object should free some large resources.
    //  3) init     --  object can try alloc resources again.
    //  4) delete   --  object life ends
    // ========================================================================
    struct  OprIF {
        virtual ~OprIF() { }
        virtual void    deleteSelf()  = 0;
        virtual int     verID()       = 0;  // the version ID used to identify interface version
        virtual ObjPtr  createObj ()  = 0;
        virtual void    deinitObj ( ObjPtr ) = 0;
        virtual void    initObj   ( ObjPtr ) = 0;
        virtual void    deleteObj ( ObjPtr ) = 0;
    };

    explicit IcObjCache( size_t max_obj_num, int tmr_gap, OprIF *opr_if );

    ~IcObjCache();

    ObjPtr  alloc();

    void    recycle( ObjPtr ptr );

    bool    isEmpty();

private:
    void  *m_obj;
    QXPACK_IC_DISABLE_COPY( IcObjCache )
};






}

#endif
