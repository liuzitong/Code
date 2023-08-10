#ifndef QXPACK_IC_QUICKLBLLAYOUTSRC_P_HXX
#define QXPACK_IC_QUICKLBLLAYOUTSRC_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//  a label layout source object
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQuickLblLayoutSrc : public QObject {
    Q_OBJECT
public:
    enum TickType {
        TickType_Major = 0, TickType_MinorHint, TickType_Minor
    };

    //! ctor
    explicit IcQuickLblLayoutSrc ( QObject *pa = Q_NULLPTR );

    //! dtor
    virtual ~IcQuickLblLayoutSrc() Q_DECL_OVERRIDE;

    void  init(
        const qint64 &st_idx,     const qint64 &end_idx,
        const qreal  &vw_ofv,     const qreal  &vw_rg_sz,
        const qreal  &tk_step,    const qreal  &ln_par_min,
        const qint64 &mj_tk_step, const qint64& mn_hint_tk_step
    );

    void    invalidate();
    bool    isValid() const;

    qint64  count() const;
    qint64  startIdx() const;
    qint64  endIdx()   const;
    qreal   viewPctPosAt( const qint64 &tk_idx, TickType *tk_type = Q_NULLPTR ) const;
    qreal   valPosAt( const qint64 &tk_idx );

    Q_SIGNAL  void  dataChanged();
private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickLblLayoutSrc )
};


}

#endif
