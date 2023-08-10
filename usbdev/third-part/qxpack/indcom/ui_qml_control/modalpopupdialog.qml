import QtQuick 2.9
import QtQml   2.2
import QtQuick.Controls 2.2
import qxpack.indcom.ui_qml_base  1.0

Item {
    id: idRoot; anchors.fill: parent;

    // ////////////////////////////////////////////////////////////////////////
    //  export property & functions
    // ////////////////////////////////////////////////////////////////////////
    function open( )
    {
        if ( backgroundVisible ) { idPopupBg.open(); }
        idPopup.open();
        if ( reqEnterEventLoop ) { IcUiQmlApi.enterEventLoop(); }
    }

    function close( )
    {
        idPopup.close();
        if ( backgroundVisible ) { idPopupBg.close(); }
    }

    property bool  reqEnterEventLoop : true;

    property bool  destroyOnClose : false;
    property alias contentItem    : idPopup.contentItem;
    property alias modal          : idPopup.modal;
    property alias closePolicy    : idPopup.closePolicy;

    property bool  backgroundVisible: false;
    property alias backgroundOpacity: idBgRect.opacity;
    property alias backgroundColor  : idBgRect.color;

    property alias popupX : idPopup.x;
    property alias popupY : idPopup.y;
    readonly property alias popupWidth : idPopup.width;
    readonly property alias popupHeight: idPopup.height;

    //! emited at the end of inner popup element onOpened()
    signal opened ( );

    //! emited at the start of inner popup element onClosed()
    signal closed ( );

    //! emited before destroy the popup
    signal aboutToDestroy ( var obj );

    // ////////////////////////////////////////////////////////////////////////
    // layout
    // ////////////////////////////////////////////////////////////////////////
    Popup { // background popup layer, this will fill in the
        id: idPopupBg; x: 0; y: 0; width: parent.width; height: parent.height; closePolicy: Popup.NoAutoClose;
        dim: false; modal: idPopup.modal; padding: 0; background: Item { }
        contentItem: Rectangle { id: idBgRect; opacity: 0.6; color: "#000000"; anchors.fill: parent; }
    }

    Popup {
        // ====================================================================
        // about the width & height.
        // normally, user item must provide the implicitWidth and implictHeight
        // Popup always use them to calc. the popup area size.
        // ====================================================================
        id: idPopup; modal: true; dim: false; background: Item {} //;Rectangle { id: idBgRect; opacity: 0.6; color: "#000000"; anchors.fill: parent; }
        padding: 0; margins: 0; closePolicy: Popup.NoAutoClose;
        x : ( parent.width - width ) / 2; y: ( parent.height - height) / 2;

        onClosed: {
            idRoot.closed();
            if ( idRoot.destroyOnClose ) {
                idRoot.aboutToDestroy( idRoot );
                idRoot.destroy();
            }
            if ( idRoot.reqEnterEventLoop ) {
                IcUiQmlApi.leaveEventLoop();
            }
        }

        onOpened: { idRoot.opened(); }

    }

    Component.onCompleted:  {
        IcUiQmlApi.aboutToQuit.connect( idRoot.close );
    }

    Component.onDestruction: {
        //console.debug("qxpack.indcom.ui.base.ModalPopupDialog.destruction()");
        IcUiQmlApi.aboutToQuit.disconnect( idRoot.close );
    }

}
