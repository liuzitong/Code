#ifndef QXPACK_IC_TMRQUEUE_HXX
#define QXPACK_IC_TMRQUEUE_HXX

#include "qxpack_ic_def.h"
#include "../common/qxpack_ic_global.hxx"
#include "../common/qxpack_ic_variant.hxx"
#include <cstdint>


namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
// the timer queue object
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcTmrQueue QXPACK_IC_FINAL {
public:
    //! ctor
    explicit IcTmrQueue( const char *queue_name = nullptr );

    //! dtor
    ~IcTmrQueue( );

    //! post single shot call
    //! @param ms    [in]  the million-seconds
    //! @param cb    [in]  the callback procedure
    //! @param p1    [in]  param1 variant object
    //! @param p2    [in]  param2 variant object
    //! @param p3    [in]  param3 variant object
    //! @param p4    [in]  param4 variant object
    //! @return the callback id, can used for cancel singleshot.
    //! @note \n
    //! cb() functions's param. 'is_valid', true means normally call, 'false' means the timer id is deprecated.\n
    //! and the callback is running in timer queue thread, not the caller thread. callback always ocurred, user \n
    //! should distinguish them by 'is_valid'
    //! @warning \n
    //!    the cb() maybe called before this function returned if 'ms' is too small !
    qxic_uid_t  singleShot(
        int ms,
        void (* cb )(
            QxPack::qxic_uid_t cb_id,
            const QxPack::IcVariant &, const QxPack::IcVariant &,
            const QxPack::IcVariant &, const QxPack::IcVariant &,
            bool is_valid
        ),
        const IcVariant &p1, const IcVariant &p2, const IcVariant &p3 = IcVariant(), const IcVariant &p4 = IcVariant()
    );

    //! invalid the singleshot
    //! @param   t_id  [in]  returned by singleShot()
    //! @note  this function make the singleshot invalid. ( the cb() will bring the 'is_valid' param. )
    void        invalidSingleShot( qxic_uid_t t_id );


    //! since 2020/09
    static const char*  disposeObjCbQueName( );

private:
    void *m_obj;
    QXPACK_IC_DISABLE_COPY( IcTmrQueue )
};

}

#endif

