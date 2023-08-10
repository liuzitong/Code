#ifndef QXPACK_IC_QSGVALUEAXISNODE_P_HXX
#define QXPACK_IC_QSGVALUEAXISNODE_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QColor>
//#include <QSGSimpleMaterial>
#include <QSGFlatColorMaterial>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QPair>
#include <QtGlobal>
#include <QVector>

#include "qxpack/indcom/ui_qml_charts/qxpack_ic_quicklbllayoutsrc_p.hxx"
#include "qxpack/indcom/algor/qxpack_ic_algor_base.hpp"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                   the value axis node
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQSGValueAxisNode : public QSGGeometryNode {
public:
    // ========================================================================
    // the line material component
    // ========================================================================
    struct QXPACK_IC_HIDDEN Material {
          enum LineType { LineType_Solid = 0 };
          QColor color; float smooth, size; LineType ln_type;

          explicit Material() { color = QColor(0,0,0); smooth = -1; size = -1; ln_type = LineType_Solid; }

          Material( float sz, float sm, const QColor &cl, LineType ln_t )
          {
             size   = IcAlgor::clamp( sz, 0.0f, sz );
             smooth = IcAlgor::clamp( sm, 0.0f, sm );
             color  = cl; ln_type = ln_t;
          }

          auto  operator == ( const Material &other ) const -> bool
          {
              return (  color == other.color && ln_type == other.ln_type &&
                     qFuzzyCompare( smooth, other.smooth ) &&
                     qFuzzyCompare( size,   other.size )
              );
          }

          inline auto  operator != ( const Material &ot ) const -> bool
          {  return !( *this == ot ); }
    };

    // ========================================================================
    // the line parameter component
    // ========================================================================
    class  QXPACK_IC_HIDDEN  LnParam {
    public :
        enum TickDir { TickDir_CW = 0, TickDir_CCW = 1, TickDir_Both = 2 };
    private: qreal m_major_size, m_minor_size, m_minor_step_size;
             qreal m_min, m_max, m_bln_min, m_bln_max;
             qreal m_head_rsvd_px, m_tail_rsvd_px;
             TickDir  m_tick_dir;
             int   m_major_cnt, m_minor_cnt, m_minor_hint_step;
             bool  m_is_vertical, m_is_show_bln;
             bool  m_show_fst_major, m_show_last_major;
    public :
        explicit LnParam() {
            m_major_size = 8; m_minor_size = 3; m_minor_step_size = 5;
            m_major_cnt  = 2; m_minor_cnt  = 0; m_minor_hint_step = 0;
            m_min = 0; m_max = 1; m_bln_min = 0; m_bln_max = 1;
            m_head_rsvd_px = m_tail_rsvd_px = 0; m_is_show_bln = true;
            m_is_vertical = false; m_tick_dir = TickDir_CW;
            m_show_fst_major = true; m_show_last_major = true;
        }
        inline auto  showFirstMajor() const -> bool { return m_show_fst_major; }
        inline auto  showLastMajor()  const -> bool { return m_show_last_major; }
        inline auto  setShowFirstMajor( bool sw ) -> void { m_show_fst_major  = sw; }
        inline auto  setShowLastMajor ( bool sw ) -> void { m_show_last_major = sw;}

        inline auto  headRsvdPx() const -> qreal { return m_head_rsvd_px; }
        inline auto  tailRsvdPx() const -> qreal { return m_tail_rsvd_px; }
        inline auto  setHeadRsvdPx( const qreal &v ) -> void { m_head_rsvd_px = IcAlgor::clamp( v, 0.0, v ); }
        inline auto  setTailRsvdPx( const qreal &v ) -> void { m_tail_rsvd_px = IcAlgor::clamp( v, 0.0, v ); }
        inline auto  isShowBln() const       -> bool  { return m_is_show_bln; }
        inline auto  setIsShowBln( bool sw ) -> void  { m_is_show_bln = sw;  }
        inline auto  majorSize() const       -> qreal { return m_major_size; }
        inline auto  minorSize() const       -> qreal { return m_minor_size; }
        inline auto  minorStepSize() const   -> qreal { return m_minor_step_size; }
        inline auto  setMajorSize ( const qreal &sz )     -> void { m_major_size      = IcAlgor::clamp( sz, 1.0, sz ); }
        inline auto  setMinorSize ( const qreal &sz )     -> void { m_minor_size      = IcAlgor::clamp( sz, 1.0, sz ); }
        inline auto  setMinorStepSize ( const qreal &sz ) -> void { m_minor_step_size = IcAlgor::clamp( sz, 1.0, sz ); }
        inline auto  min() const -> qreal { return m_min; }
        inline auto  max() const -> qreal { return m_max; }
        inline auto  setMin( const qreal &v ) -> void { m_min = v; IcAlgor::swapIf( m_min, m_max, m_min > m_max ); }
        inline auto  setMax( const qreal &v ) -> void { m_max = v; IcAlgor::swapIf( m_min, m_max, m_min > m_max ); }
        inline auto  blnMin() const -> qreal { return m_bln_min; }
        inline auto  blnMax() const -> qreal { return m_bln_max; }
        inline auto  setBlnMin( const qreal &v ) -> void { m_bln_min = v; IcAlgor::swapIf( m_bln_min, m_bln_max, m_bln_min > m_bln_max ); }
        inline auto  setBlnMax( const qreal &v ) -> void { m_bln_max = v; IcAlgor::swapIf( m_bln_min, m_bln_max, m_bln_min > m_bln_max ); }
        inline auto  majorTickCnt()  const -> int { return m_major_cnt; }
        inline auto  minorTickCnt()  const -> int { return m_minor_cnt; }
        inline auto  minorHintStep() const -> int { return m_minor_hint_step; }
        inline auto  setMajorTickCnt ( int cnt ) -> void { m_major_cnt = IcAlgor::clamp( cnt, 2, cnt ); }
        inline auto  setMinorTickCnt ( int cnt ) -> void { m_minor_cnt = IcAlgor::clamp( cnt, 0, cnt ); }
        inline auto  setMinorHintStep( int st  ) -> void { m_minor_hint_step = IcAlgor::clamp( st, 0, st ); }
        inline auto  isVertical() const       -> bool    { return m_is_vertical; }
        inline auto  setVertical( bool sw )   -> void    { m_is_vertical = sw; }
        inline auto  tickDir() const          -> TickDir { return m_tick_dir; }
        inline auto  setTickDir( TickDir td ) -> void    { m_tick_dir = td; }

        auto  operator == ( const LnParam &ot ) const -> bool
        {
            return ( m_is_vertical == ot.m_is_vertical ) &&
               ( m_major_cnt == ot.m_major_cnt   ) && ( m_minor_cnt == ot.m_minor_cnt ) &&
               ( m_minor_hint_step == ot.m_minor_hint_step ) && ( m_tick_dir == ot.m_tick_dir ) &&
               ( m_is_show_bln == ot.m_is_show_bln ) &&
               ( m_show_fst_major  == ot.m_show_fst_major ) &&
               ( m_show_last_major == ot.m_show_last_major ) &&
               qFuzzyCompare( m_min, ot.m_min )  &&  qFuzzyCompare( m_max, ot.m_max )  &&
               qFuzzyCompare( m_bln_min, ot.m_bln_min ) && qFuzzyCompare( m_bln_max, ot.m_bln_max ) &&
               qFuzzyCompare( m_major_size, ot.m_major_size ) && qFuzzyCompare( m_minor_size, ot.m_minor_size ) &&
               qFuzzyCompare( m_minor_step_size, ot.m_minor_step_size ) &&
               qFuzzyCompare( m_head_rsvd_px, ot.m_head_rsvd_px )  &&  qFuzzyCompare( m_tail_rsvd_px, ot.m_tail_rsvd_px );
        }

        inline auto  operator != ( const LnParam &ot ) const -> bool
        { return !( *this == ot ); }
    };

    // ========================================================================
    // the major pixel position
    // ========================================================================
    class QXPACK_IC_HIDDEN  MajorTickPxPos {
    private:
        QVector<qreal> m_pos;
    public :
        explicit MajorTickPxPos ( ) {  }

        MajorTickPxPos( int rsvd_sz ) { m_pos.reserve( rsvd_sz > 0 ? rsvd_sz : 4 ); }

        MajorTickPxPos( const MajorTickPxPos &other ) { m_pos = other.m_pos; }

        auto  operator = ( const MajorTickPxPos &other ) -> MajorTickPxPos &
        { m_pos = other.m_pos; return *this; }

        inline auto  isEmpty() const -> bool { return m_pos.isEmpty(); }
        inline auto  vect() -> QVector<qreal>& { return m_pos; }
    };

private:
    QSGGeometry  m_geom;
    QSGFlatColorMaterial  m_flat_mater;
    Material m_mater;
protected:
    auto  clearGeom ( IcQuickLblLayoutSrc* ) -> void;
    auto  qsg_updGeomVert( const QRectF &bounds, const LnParam &ln_param, const QRectF &vw_port, const Material &mater,  IcQuickLblLayoutSrc *lbl_layout ) -> void;
    auto  qsg_updGeomHori( const QRectF &bounds, const LnParam &ln_param, const QRectF &vw_port, const Material &mater,  IcQuickLblLayoutSrc *lbl_layout ) -> void;
public :
    explicit IcQSGValueAxisNode( const Material &mater );
    virtual ~IcQSGValueAxisNode() Q_DECL_OVERRIDE;

    auto  qsg_updateMaterial( const Material &mater ) -> bool;

    inline auto qsg_updateGeometry( const QRectF &bounds, const LnParam &ln_param, const QRectF &vw_port, const Material &mater,  IcQuickLblLayoutSrc *lbl_layout ) -> void
    {
        if ( ! ln_param.isVertical() ) {
            qsg_updGeomHori( bounds, ln_param, vw_port, mater, lbl_layout );
        } else {
            qsg_updGeomVert( bounds, ln_param, vw_port, mater, lbl_layout );
        }
    }
};


}
#endif
