import QtQuick 2.9

// ////////////////////////////////////////////////////////////////////////////
// the page bse is used for provide a normal simple page item
// //////////////////////////////////////////////////////////////////////////// 
Item {
    id: idRoot; implicitWidth:160; implicitHeight:120;

    // ========================================================================
    //! pageInit signal is need emited after the item has been initailized 
    // ========================================================================
    signal pageInit ( );  

    // ========================================================================
    //! pageDeinit signal is need emited after the item has been deinitailized
    // ========================================================================
    signal pageDeinit ( );

    // ========================================================================
    /*! 
     * @brief the request signal is used for submit command to parent
     * @param  req_str [in] the custom command string
     * @param  req_arg [in] the variant parameter 
     */
    // ========================================================================
    signal request ( string req_str, var req_arg ); 

    // ========================================================================
    //! pre-do page offline before called pageDeinit()
    // @details  this function can override by sub item
    // ========================================================================
    function  doOffline ( ) { } 

    // ========================================================================
    //! this item page name, used for custom defined string
    // ========================================================================
    property string pageName: "";
}
