#ifndef QXPACK_IC_QSGVALUEAXISNODE_P_HXX
#define QXPACK_IC_QSGVALUEAXISNODE_P_HXX

#include "qxpack/indcom/common/qxpack_ic_def.h"

#include <QSGRenderNode>
#include <QQuickItem>
#include <QColor>
#include <QList>
#include <QPointF>
#include <QRectF>
#include <QPair>
#include <QtGlobal>
#include <QVector>

#include "qxpack/indcom/ui_qml_charts/qxpack_ic_quicklbllayoutsrc_p.hxx"
#include "qxpack/indcom/algor/qxpack_ic_algor_base.hpp"
#include <cmath>

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
//
//                   the value axis node
//
// ////////////////////////////////////////////////////////////////////////////
class QXPACK_IC_HIDDEN  IcQSGValueAxisNode : public QSGRenderNode {
public:
    // ========================================================================
    // the line material component
    // ========================================================================
    struct QXPACK_IC_HIDDEN Material {
          enum LineType { LineType_Solid = 0 };
          QColor color; float size; LineType ln_type;

          explicit Material() { color = QColor(0,0,0); size = 1; ln_type = LineType_Solid; }

          Material( float sz, const QColor &cl, LineType ln_t )
          {
             size   = IcAlgor::clamp( sz, 0.0f, sz );
             color  = cl; ln_type = ln_t;
          }

          auto  operator == ( const Material &other ) const -> bool
          {
              return (
                  color == other.color && ln_type == other.ln_type &&  qFuzzyCompare( size, other.size )
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
            m_is_vertical    = false; m_tick_dir  = TickDir_CW;
            m_show_fst_major = true;  m_show_last_major = true;
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
    // the tick information
    // ========================================================================
    class QXPACK_IC_HIDDEN  TickInfo {
    private:
        qreal    m_vrg_min,  m_vrg_max;    // tick visble value range
        qreal    m_brg_min,  m_brg_max;    // base line value range
        qreal    m_btk_step, m_mj_tk_step; // base tick step ( the minimal value step ),
        qreal    m_nr_min_tk_idx, m_nr_max_tk_idx; // nearest tick index in view port,
                                            // NOTE: the tick index is begin with parameter's minimal value.
        qintptr  m_btk_cnt;
    public :
        explicit TickInfo ()
        {
            m_vrg_min = 0; m_vrg_max = 1; m_brg_min = 0; m_brg_max = 1;
            m_btk_step = 0; m_mj_tk_step = 1;
            m_nr_min_tk_idx = m_nr_max_tk_idx = 0;
        }

        TickInfo( const qreal &vw_min, const qreal &vw_max, const IcQSGValueAxisNode::LnParam &ln_par )
        {
            // [ m_vrg_min, m_vrg_max ] is used for ticks ( visible for user )
            if ( vw_min <= vw_max ) {
                m_vrg_min = ( ln_par.min() <= vw_min ? vw_min : ( ln_par.min() <= vw_max ? ln_par.min() : vw_max + 1.0 ));
                m_vrg_max = ( ln_par.max() >= vw_max ? vw_max : ( ln_par.max() >= vw_min ? ln_par.max() : vw_min - 1.0 ));
            } else {
                m_vrg_min = ln_par.min();
                m_vrg_max = ln_par.max();
            }

            // [ m_brg_min, m_brg_max ] is used for base line
            if ( vw_min <= vw_max ) {
                m_brg_min = ( ln_par.blnMin() <= vw_min ? vw_min : ( ln_par.blnMin() <= vw_max ? ln_par.blnMin() : vw_max + 1.0 ));
                m_brg_max = ( ln_par.blnMax() >= vw_max ? vw_max : ( ln_par.blnMax() >= vw_min ? ln_par.blnMax() : vw_min - 1.0 ));
            } else {
                m_brg_max = ln_par.blnMax();
                m_brg_min = ln_par.blnMin();
            }

            m_btk_step = m_mj_tk_step = 0; m_btk_cnt = -1;
            m_nr_max_tk_idx = m_nr_max_tk_idx = -1;

            if ( isValid() ) {
                m_mj_tk_step = ( ln_par.max() - ln_par.min() ) / ( ln_par.majorTickCnt() - 1 );
                m_btk_cnt  = ( ln_par.majorTickCnt() - 1 ) * ln_par.minorTickCnt() + ln_par.majorTickCnt();
                m_btk_step = m_mj_tk_step / ( ln_par.minorTickCnt() + 1 );
                auto vrg_min_t = m_vrg_min - ln_par.min();
                auto vrg_max_t = m_vrg_max - ln_par.min();
                std::modf( vrg_min_t / m_btk_step, &m_nr_min_tk_idx );
                std::modf( vrg_max_t / m_btk_step, &m_nr_max_tk_idx );
                if ( m_nr_max_tk_idx * m_btk_step < vrg_max_t ) { m_nr_max_tk_idx ++; }
            }
        }

        inline auto  isValid() const      -> bool { return m_brg_min < m_brg_max; }
        inline auto  hasTickInVrg() const -> bool { return m_nr_max_tk_idx >= m_nr_min_tk_idx; }
        inline auto  baseTickStep() const -> const qreal& { return m_btk_step; }
        inline auto  vrgMin() const -> const qreal& { return m_vrg_min; }
        inline auto  vrgMax() const -> const qreal& { return m_vrg_max; }
        inline auto  brgMin() const -> const qreal& { return m_brg_min; }
        inline auto  brgMax() const -> const qreal& { return m_brg_max; }
        inline auto  nrMinTickIdx() const -> const qreal& { return m_nr_min_tk_idx; }
        inline auto  nrMaxTickIdx() const -> const qreal& { return m_nr_max_tk_idx; }
        inline auto  nrMinTickIdxInt() const -> qint64 { return qint64( m_nr_min_tk_idx ); }
        inline auto  nrMaxTickIdxInt() const -> qint64 { return qint64( m_nr_max_tk_idx ); }
    };



private:
    Material     m_mater;
    LnParam      m_ln_par;
    QRectF       m_vw_port;
    TickInfo     m_tick_info;
    QQuickItem  *m_item;

    bool  m_is_ln_par_chg;
protected:

public :
    explicit IcQSGValueAxisNode( QQuickItem *m_item );
    virtual ~IcQSGValueAxisNode() Q_DECL_OVERRIDE;

    inline auto  setMaterial( const Material &mat ) -> void
    {  m_mater = mat; }

    inline auto  setLnParam ( const LnParam &ln ) -> void
    {  m_ln_par = ln;  m_is_ln_par_chg = true; }

    inline auto  setViewport( const QRectF &vw ) -> void
    {  m_vw_port = vw; }

    inline auto  estmTickInfo( ) -> void
    { m_tick_info = TickInfo( m_vw_port.y(), m_vw_port.bottom(), m_ln_par );  }

    auto  estmLabelLayout( IcQuickLblLayoutSrc *lbl_layout ) -> void;
};


}
#endif
