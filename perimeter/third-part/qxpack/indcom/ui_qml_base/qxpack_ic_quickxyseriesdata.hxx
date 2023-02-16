#ifndef QXPACK_IC_QUICKXYSERIESDATA_HXX
#define QXPACK_IC_QUICKXYSERIALDATA_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QObject>
#include <QList>
#include <QPointF>
#include <QPair>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
/*!
 * @brief provide the wave line data, running in UI thread only
 */
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_API  IcQuickXYSeriesData : public QObject {
    Q_OBJECT
public:
    enum UseMode { UseMode_Normal = 0 };

    //! ctor empty
    explicit IcQuickXYSeriesData( );

    //! dtor
    virtual ~IcQuickXYSeriesData() Q_DECL_OVERRIDE;

    //! ctor with mode
    IcQuickXYSeriesData( UseMode );

    //! check if internal data is same
           bool  isEqulTo( const IcQuickXYSeriesData &other ) const;

    //! attach to another object
           bool  attach ( const IcQuickXYSeriesData &other );

    //! check if it is null ( means create with empty ctor )
           bool  isNull( ) const;

    //! clear all data
    //! @note  the view rectangle is not cleared!
    Q_SLOT void  clear();

    //! check current data size()
    Q_SLOT int   size() const;

    //! get all data
    Q_SLOT QList<QPointF>  data() const;

    //! return the lower and upper bound of spec. x coord
    Q_SLOT QPair<QPointF,QPointF>  findBound( const qreal &x, bool &is_found );

    //! set data
    //! @note  each element should be a normalized value !
    Q_SLOT void  setData( const QList<QPointF>& );

    //! remove front and push back data
    //! @param  num    [in] if num > 0, means remove spec. number of front elements
    //! @param  new_pt [in] the new pt
    Q_SLOT void  rmvFrontAndPushBack( int num, const QPointF& );

    //! remove front and push back data
    Q_SLOT void  rmvFrontAndPushBack( int num, const QList<QPointF> & );

    //! remove front data
    Q_SLOT void  removeFront( int );


    Q_SIGNAL void  dataChanged();

private:
    void *m_obj;
    Q_DISABLE_COPY( IcQuickXYSeriesData )
};


}

#endif
