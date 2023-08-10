#ifndef QXPACK_IC_QSGLINESERIESNODE_P_HXX
#define QXPACK_IC_QSGLINESERIESNODE_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"
#include <QColor>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QtGlobal>
#include <QSGRenderNode>
#include <QQuickItem>
#include <QDebug>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                   the line series node
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQSGLineSeriesNode : public QSGRenderNode {
public:
    enum DataType {
        DataType_Unknown = 0,  DataType_Vector, DataType_List
    };

    // ========================================================================
    // the line material component
    // ========================================================================
    struct QXPACK_IC_HIDDEN Material {
          enum LineType { LineType_Solid = 0 };
          QColor color; float size; LineType ln_type;

          explicit Material() {
              color = QColor("black"); size = 1; ln_type = LineType_Solid;
          }

          Material( float sz, const QColor &cl, LineType ln_t )
          {
             size   = ( sz >= 0.0f ? sz : 0.0f );
             color  = cl; ln_type = ln_t;
          }

          auto  operator == ( const Material &other ) const -> bool
          {
           return (  color == other.color && ln_type == other.ln_type &&
                     qFuzzyCompare( size, other.size  )
                   );
           }

           inline auto  operator != ( const Material &other ) const -> bool
           { return !( *this == other ); }
    };


protected:
    Material  m_mater;
    QRectF    m_vw_port;

    QVector<float>  m_vec_data;
    QList<QPointF>  m_lst_data;
    float m_x_step, m_x_ofv;

    QQuickItem *m_item;

    DataType  m_dat_type; size_t m_req_size;
    bool  m_is_data_chg;
public :
    explicit IcQSGLineSeriesNode( QQuickItem *item );
    virtual ~IcQSGLineSeriesNode() Q_DECL_OVERRIDE;

    inline auto  setMaterial( const Material &mat ) -> void
    { m_mater = mat; }

    inline auto  setViewport( const QRectF &vw ) -> void
    { m_vw_port = vw; }

    inline auto  setData( const QList<QPointF> &lst ) -> void
    { m_lst_data = lst; m_vec_data = QVector<float>(); m_is_data_chg = true; m_dat_type = DataType_List; m_req_size = size_t( lst.size()) * sizeof(float)*2; markDirty( QSGNode::DirtyGeometry | QSGNode::DirtyMaterial ); }

    inline auto  setData( const QVector<float> &vec, float x_ofv, float x_step ) -> void
    { m_vec_data = vec; m_lst_data = QList<QPointF>(); m_x_step = x_step; m_x_ofv = x_ofv; m_is_data_chg = true; m_dat_type = DataType_Vector; m_req_size = size_t(vec.size()) * sizeof(float); markDirty( QSGNode::DirtyGeometry | QSGNode::DirtyMaterial ); }

    static auto  createByContext( QQuickItem *item ) -> IcQSGLineSeriesNode*;
};


}

#endif
