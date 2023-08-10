! contains( IOLCALC_MODULES, iolcalc_qxpack_ic ) {
    IOLCALC_MODULES *= iolcalc_qxpack_ic
    CONFIG *= c++11

    include( $$PWD/../third-part/qxpackic-build/qxpack/indcom/qxpackic_all_static.pri )

}
