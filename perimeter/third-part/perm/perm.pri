
! contains( PERM_MODULES, perm ) {
  PERM_MODULES *= perm

  QT *= quick qml sql
  DEFINES *= PERM_CFG_STATIC

  INCLUDEPATH *= $$PWD/../
  QML_IMPORT_PATH  += $$PWD/../
  QML_IMPORT_PATH  += $$PWD/../../

  HEADERS += \
    $$PWD/common/perm_def.h \
    $$PWD/common/perm_dequetemp.hpp \
    $$PWD/common/perm_dyncinit_priv.hxx \
    $$PWD/common/perm_guns.hxx \
    $$PWD/common/perm_logging.hxx \
    $$PWD/common/perm_memcntr.hxx \
    $$PWD/common/perm_pimplprivtemp.hpp \
    $$PWD/common/perm_semtemp.hpp \
    \
    $$PWD/perm_mod.hxx \
    \
    $$PWD/viewmodel/perm_permmgrvm.hxx \
    $$PWD/viewmodel/perm_userloginvm.hxx \
    $$PWD/viewmodel/perm_usermgrvm.hxx \
    $$PWD/viewmodel/perm_usergrpmgrvm.hxx \
    $$PWD/viewmodel/perm_userinfochgvm.hxx \
    $$PWD/viewmodel/perm_userpermlistvm.hxx \
    $$PWD/viewmodel/perm_usergrppermlistvm.hxx \
    $$PWD/viewmodel/perm_usergrplistvm.hxx \
    $$PWD/viewmodel/perm_newuservm.hxx \
    $$PWD/viewmodel/perm_newusergrpvm.hxx \
    $$PWD/viewmodel/perm_moduservm.hxx \
    $$PWD/viewmodel/perm_modusergrpvm.hxx \
    \
    $$PWD/service/perm_editableuserdichgpwdinfo.hxx \
    $$PWD/service/perm_editableusergrpinfo.hxx \
    $$PWD/service/perm_editableuserpermlistinfo.hxx \
    $$PWD/service/perm_editableusergrppermlistinfo.hxx \
    $$PWD/service/perm_editableusergrplistinfo.hxx \
    $$PWD/service/perm_editableuserinfo.hxx \
    $$PWD/service/perm_userpermfiltersvc.hxx \
    $$PWD/service/perm_permbroker.hxx \
    $$PWD/service/perm_localpwdmgrsvc.hxx \
    $$PWD/service/perm_specuserpermlistinfo.hxx \
    $$PWD/service/perm_specusergrplistinfo.hxx \
    $$PWD/service/perm_usergrpfiltersvc.hxx \
    $$PWD/service/perm_specusergrppermlistinfo.hxx \
    $$PWD/service/perm_usergrppermfiltersvc.hxx \
    $$PWD/service/perm_aesencryption.hxx \
    $$PWD/service/perm_userandperminfo.hxx \
    $$PWD/service/perm_rpcclient.hxx \
    $$PWD/service/perm_svrprocess.hxx \
    \
    $$PWD/model/perm_userlistinfo.hxx \
    $$PWD/model/perm_usergrplistinfo.hxx \
    $$PWD/model/perm_userinfodata.hxx \
    $$PWD/model/perm_usergroupinfodata.hxx \
    $$PWD/model/perm_privpermpair.hxx \
    $$PWD/model/perm_passwordvalidator.hxx \
    $$PWD/model/perm_groupsimpledesc.hxx \
    $$PWD/model/perm_typedef.hxx \

  SOURCES += \
    $$PWD/common/perm_logging.cxx \
    $$PWD/common/perm_memcntr.cxx \
    \
    $$PWD/perm_mod.cxx \
    \
    $$PWD/viewmodel/perm_permmgrvm.cxx \
    $$PWD/viewmodel/perm_userloginvm.cxx \
    $$PWD/viewmodel/perm_usermgrvm.cxx \
    $$PWD/viewmodel/perm_usergrpmgrvm.cxx \
    $$PWD/viewmodel/perm_userinfochgvm.cxx \
    $$PWD/viewmodel/perm_userpermlistvm.cxx \
    $$PWD/viewmodel/perm_usergrppermlistvm.cxx \
    $$PWD/viewmodel/perm_newuservm.cxx \
    $$PWD/viewmodel/perm_usergrplistvm.cxx \
    $$PWD/viewmodel/perm_newusergrpvm.cxx \
    $$PWD/viewmodel/perm_moduservm.cxx \
    $$PWD/viewmodel/perm_modusergrpvm.cxx \
    \
    $$PWD/service/perm_editableuserdichgpwdinfo.cxx \
    $$PWD/service/perm_editableusergrpinfo.cxx \
    $$PWD/service/perm_editableuserpermlistinfo.cxx \
    $$PWD/service/perm_editableusergrppermlistinfo.cxx \
    $$PWD/service/perm_editableusergrplistinfo.cxx \
    $$PWD/service/perm_editableuserinfo.cxx \
    $$PWD/service/perm_userpermfiltersvc.cxx \
    $$PWD/service/perm_permbroker.cxx \
    $$PWD/service/perm_localpwdmgrsvc.cxx \
    $$PWD/service/perm_specuserpermlistinfo.cxx \
    $$PWD/service/perm_specusergrplistinfo.cxx \
    $$PWD/service/perm_usergrpfiltersvc.cxx \
    $$PWD/service/perm_specusergrppermlistinfo.cxx \
    $$PWD/service/perm_usergrppermfiltersvc.cxx \
    $$PWD/service/perm_aesencryption.cxx \
    $$PWD/service/perm_userandperminfo.cxx \
    $$PWD/service/perm_rpcclient.cxx \
    $$PWD/service/perm_svrprocess.cxx \
    \
    $$PWD/model/perm_userlistinfo.cxx \
    $$PWD/model/perm_usergrplistinfo.cxx \
    $$PWD/model/perm_userinfodata.cxx \
    $$PWD/model/perm_usergroupinfodata.cxx \
    $$PWD/model/perm_privpermpair.cxx \
    $$PWD/model/perm_passwordvalidator.cxx \
    $$PWD/model/perm_groupsimpledesc.cxx \


    include($$PWD/local-part/localpart.pri)
    include($$PWD/view/perm_ui_view.pri)

     include($$PWD/third-part/jrpcplat/jrpcplatcli.pri)
    isEmpty(DESTDIR){ DESTDIR = $$OUT_PWD }
    PERM_MAKE_POST_LINK =  cp -r $$PWD/resource/*        $$DESTDIR/resource \
                         & cp -r $$PWD/third-part/*      $$DESTDIR
}


