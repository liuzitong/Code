QT += opengl

HEADERS = $$PWD/glwin2.hxx \
          $$PWD/../oglcalc/oglcalc.hxx \
          $$PWD/../oglcalc/qtoglfuncbroker.hxx \


SOURCES = $$PWD/main.cpp \
          $$PWD/glwin2.cxx \
          $$PWD/../oglcalc/oglcalc.cxx \
          $$PWD/../oglcalc/qtoglfuncbroker.cxx \


#target.path = $$[QT_INSTALL_EXAMPLES]/opengl/computegles31
#INSTALLS += target

DEFINES += APP_PWD=\\\"$${PWD}\\\"

