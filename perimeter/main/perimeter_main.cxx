#ifndef PERIMETER_MAIN_CXX
#define PERIMETER_MAIN_CXX

#include "perimeter_main.hxx"
#include "../Control/Calendar/perimeter_calendarvm.hxx"
#include "perimeter/main/viewModel/patientVm.h"
#include "perimeter/main/viewModel/patientListModelVm.h"
#include <perimeter/main/viewModel/programListModelVm.h>
#include <perimeter/main/viewModel/programVm.h>
#include <perimeter/main/viewModel/checkResultVm.h>
#include <perimeter/main/viewModel/analysislobbylistvm.h>
#include <perimeter/main/viewModel/progressanalysislistvm.h>
#include <perimeter/main/viewModel/general_analysis_vm.h>
#include <perimeter/main/viewModel/visionFieldIslandVm.h>
#include <perimeter/main/object_wrapper.h>

// services list

namespace Perimeter {

// ////////////////////////////////////////////////////////////////////////////
//  wrap API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// ctor
// ============================================================================
Main :: Main ( QObject *pa ) : QxPack::IcModMgrBase( pa )
{



}

// ============================================================================
// dtor
// ============================================================================
Main :: ~Main ( )
{

}

// ============================================================================
// register types
// ============================================================================
void  Main :: registerTypes( QxPack::IcAppCtrlBase *app_base )
{
    // ------------------------------------------------------------------------
    // check param.
    // ------------------------------------------------------------------------
    if ( app_base == Q_NULLPTR ) { return; }
    QxPack::IcObjMgr *obj_mgr = app_base->objMgr();
    if ( obj_mgr  == Q_NULLPTR ) { return; }

    // ------------------------------------------------------------------------
    // register all public types of this module
    // ------------------------------------------------------------------------
    obj_mgr->registerType<Perimeter::CalendarVm>(QStringLiteral("Perimeter::CalendarVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::PatientVm>(QStringLiteral("Perimeter::PatientVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::PatientListModelVm>(QStringLiteral("Perimeter::PatientListModelVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::ProgramListModelVm>(QStringLiteral("Perimeter::ProgramListModelVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::StaticProgramVm>(QStringLiteral("Perimeter::StaticProgramVM"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::DynamicProgramVm>(QStringLiteral("Perimeter::DynamicProgramVM"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::StaticCheckResultVm>(QStringLiteral("Perimeter::StaticCheckResultVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::DynamicCheckResultVm>(QStringLiteral("Perimeter::DynamicCheckResultVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::AnalysisLobbyListVm>(QStringLiteral("Perimeter::AnalysisLobbyListVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::ProgressAnalysisListVm>(QStringLiteral("Perimeter::ProgressAnalysisListVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::StaticAnalysisVm>(QStringLiteral("Perimeter::StaticAnalysisVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::QHeightMapSurfaceDataProxyWrapper>(QStringLiteral("Perimeter::QHeightMapSurfaceDataProxyWrapper"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::DynamicAnalysisVm>(QStringLiteral("Perimeter::DynamicAnalysisVm"), Q_NULLPTR);
    obj_mgr->registerType<Perimeter::VisionFieldIslandVm>(QStringLiteral("Perimeter::VisionFieldIslandVm"), Q_NULLPTR);


}


}

#endif
