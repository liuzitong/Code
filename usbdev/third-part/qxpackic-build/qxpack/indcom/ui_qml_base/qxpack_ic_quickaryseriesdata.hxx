#ifndef QXPACK_IC_QUICKARYSERIESDATA_HXX
#define QXPACK_IC_QUICKARYSERIALDATA_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QList>
#include <QPointF>
#include <QPair>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief provide the wave line data, running in UI thread only
 * @note this is used just the time line series.
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickArySeriesData : public QObject {
    Q_OBJECT
public:
    enum UseMode { UseMode_Normal = 0 };

    struct SeriesData {
        QVector<float>  vec; float  x_ofv,  step;
        SeriesData() : x_ofv(0), step(0) { }
        SeriesData( const QVector<float> &v, float ofv, float st ) : vec(v), x_ofv(ofv), step(st) { }
    };

    //! ctor empty
    explicit IcQuickArySeriesData( );

    //! dtor
    virtual ~IcQuickArySeriesData() Q_DECL_OVERRIDE;

    //! ctor with mode
    IcQuickArySeriesData( UseMode );

    //! check if internal data is same
           bool  isEqulTo( const IcQuickArySeriesData &other ) const;

    //! attach to another object
           bool  attach  ( const IcQuickArySeriesData &other );

    //! check if it is null ( means create with empty ctor )
           bool  isNull( ) const;

    //! clear all data
    //! @note  the view rectangle is not cleared!
    Q_SLOT void  clear();

    //! check current data size()
    Q_SLOT int   size() const;

    //! get all data
    Q_SLOT SeriesData  data() const;

    //! set data
    //! @note  each element should be a normalized value !
    Q_SLOT void  setData( const SeriesData& );

    //! return the lower and upper bound of spec. x pos
    Q_SLOT QPair<QPointF,QPointF>  findBound( const qreal &x, bool &is_found );


    Q_SIGNAL void  dataChanged();

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickArySeriesData )
};


}

#endif
