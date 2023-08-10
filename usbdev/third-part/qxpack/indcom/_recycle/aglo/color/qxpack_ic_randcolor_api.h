#ifndef QXPACK_IC_RANDCOLOR_API_H
#define QXPACK_IC_RANDCOLOR_API_H

#include "../qxpack_ic_aglo_def.h"

#ifdef __cplusplus
extern "C" {
#endif

//! the random color generator by randomize Hue
//! @param seed  [in]  the random value seed, 0 means default seed.
//! @param s     [in]  the HSV.Saturation [0,1]
//! @param v     [in]  the HSV.Value      [0,1]
//! @param Cb    [in] the callback function.
//! @param cb_ctxt [in] the callback context
//! @return none
//! @note the callback function 'rgba' used format 0xRRGGBBAA, and callback return 0 for stop.
//! @note  HSV  S control the brightness, V control the darkness.
QXPACK_IC_HIDDEN void  QxPack_IcRandColorByHue(
    int  seed, double s, double v, int (* Cb )( void*, int r, int g, int b ), void *cb_ctxt
);

#ifdef __cplusplus
}
#endif

#endif
