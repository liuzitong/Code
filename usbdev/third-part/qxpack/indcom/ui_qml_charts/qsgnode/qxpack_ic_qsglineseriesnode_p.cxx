#ifndef QXPACK_IC_QSGLINESERIESNODE_P_CXX
#define QXPACK_IC_QSGLINESERIESNODE_P_CXX

#include "qxpack_ic_qsglineseriesnode_p.hxx"
#include "qxpack_ic_qsglineseriesnode_glsl_4_0_p.hxx"

namespace QxPack {

// ////////////////////////////////////////////////////////////////////////////
// QSG Line Series Node
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor ( create it in QSG thread )
// ============================================================================
IcQSGLineSeriesNode :: IcQSGLineSeriesNode( QQuickItem *item )
{
    m_item = item;
    m_x_ofv = 0; m_x_step = 0;
    m_dat_type = DataType_Unknown;
    m_is_data_chg = false; m_req_size = 0;
}

// ============================================================================
// dtor
// ============================================================================
IcQSGLineSeriesNode :: ~IcQSGLineSeriesNode( )
{
}

// ============================================================================
// [ static ] create by context
// ============================================================================
auto     IcQSGLineSeriesNode :: createByContext( QQuickItem *item ) -> IcQSGLineSeriesNode*
{
    return new IcQSGLineSeriesNodeGLSL4_0( item );
}


}
#endif
