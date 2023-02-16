
! contains( PERIMETER_MODULES, base ) {
  PERIMETER_MODULES *= base

  DEFINES *= PERIMETER_USE_QT5_INFO

  contains( QT, core ) {
      DEFINES *= PERIMETER_QT5_ENABLED
  }
  contains( CONFIG, qt ) {
      DEFINES *= PERIMETER_QT5_ENABLED
  }

  HEADERS += \
    $$PWD/common/perimeter_def.h \
    $$PWD/common/perimeter_memcntr.hxx \
    $$PWD/common/perimeter_logging.hxx \
    $$PWD/common/perimeter_pimplprivtemp.hpp \
    $$PWD/service/perimeter_appsettingssvc.hxx \
    $$PWD/service/perimeter_objmgrsvc.hxx \
    $$PWD/service/perimeter_msgbussvc.hxx \
    $$PWD/service/perimeter_cfmbussvc.hxx \

  SOURCES += \
    $$PWD/common/perimeter_memcntr.cxx \
    $$PWD/common/perimeter_logging.cxx \
    $$PWD/service/perimeter_appsettingssvc.cxx \
    $$PWD/service/perimeter_objmgrsvc.cxx \
    $$PWD/service/perimeter_msgbussvc.cxx \
    $$PWD/service/perimeter_cfmbussvc.cxx \

}
