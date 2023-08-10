#ifndef QXPACK_IC_NDCTHR_API_C
#define QXPACK_IC_NDCTHR_API_C

#include "qxpack_ic_ndcthr_api.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdio.h>

typedef int cint_t;

// ////////////////////////////////////////////////////////////////////////////
//                      local functions
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// the elem position in triu array
// ============================================================================
inline static int gTriuElemPos( int row, int col, int dim  )
{
    // ------------------------------------------------------------------------
    // formular: pos = ( i - 1 ) ( 2*n - i + 2 ) / 2 + j - i  ( i,j > 0 and i <= j )
    // if i > j, return the reflected position..
    // ------------------------------------------------------------------------
    int i, j;
    if ( row <= col ) { i = row + 1; j = col + 1; } else { i = col + 1; j = row + 1; }
    return (( i - 1 ) * ( dim + dim - i + 2 ) / 2 + j - i );
}

// ============================================================================
// the elem position in trii array
// ============================================================================
inline static int gTrilElemPos( int row, int col, int dim )
{
    (void)(dim);

    // ------------------------------------------------------------------------
    // formular: pos = i * ( i - 1 ) / 2 + j - 1    ( i,j>0 and i <= j )
    // ------------------------------------------------------------------------
    int i, j;
    if ( row >= col ) { i = row + 1; j = col + 1; } else { i = col + 1; j = row + 1; }
    return ( i * ( i - 1 ) / 2 + j - 1 );
}

// ============================================================================
// estimate class array size
// ============================================================================
inline static int gClasArySize( int dim )
{
    return dim * sizeof( cint_t ) * 2;  // [ obj node area ][ root & middle node ]
}

// ============================================================================
// estimate dim by class array size
// ============================================================================
inline static int gDimByClasSize( int sz )
{
    return  sz / sizeof(cint_t) / 2;
}


// ////////////////////////////////////////////////////////////////////////////
//                      implement API
// ////////////////////////////////////////////////////////////////////////////
// ============================================================================
// generate the class array data from distance triu matrix
// ============================================================================
QXPACK_IC_HIDDEN int  QxPack_IcNdcThrAryFromTriu (
    void *clas_ary, int clas_ary_sz, const float *dist_triu, int dist_triu_dim, float dist_thresh
) {
    // ------------------------------------------------------------------------
    //                            class array memory map
    // [ obj node area ][ root & middle node area & class map ID ]
    // ------------------------------------------------------------------------
    if ( dist_triu == NULL || dist_triu_dim > INT_MAX / 2 / sizeof( cint_t ) ) { return -1; }

    int clas_req_sz = gClasArySize( dist_triu_dim );
    if ( clas_ary == NULL || clas_ary_sz < 1 )                { return clas_req_sz; }
    if ( clas_ary_sz < clas_req_sz )                          { return -2; }
    if ( (( uintptr_t )( clas_ary ) % sizeof(cint_t )) != 0 ) { return -9; } // invalid address...

    // -------------------------------------------------------------------------
    //                            initialize stage
    // -------------------------------------------------------------------------
    cint_t  e_num = ( cint_t )( dist_triu_dim );
    cint_t *ca    = & (( cint_t *) clas_ary )[0];            // [ obj class node ][ root & middle node ]
    cint_t *ra    = & (( cint_t *) clas_ary )[ e_num ];
    for ( cint_t i = 0; i < e_num; i ++ ) { ca[ i ] = i; ra[ i ] = -1; }

    // ------------------------------------------------------------------------
    //                            process stage
    // ------------------------------------------------------------------------
    cint_t  cls_top = e_num;  cint_t dim = ( cint_t )( dist_triu_dim );
    for (
        cint_t e = 0, e_triu_pos = 0;
        e < e_num - 1;
        e_triu_pos += dim - e, e ++
    ) {
        // --------------------------------------------------------------------
        // find 'e' 's root index (e_rt)
        // --------------------------------------------------------------------
        cint_t e_rt = e; for (; ca[ e_rt ] != e_rt; e_rt = ca[ e_rt ] ) { }
        if ( e_rt < e_num ) { //[ is a leaf node, create new node ]
            ca[ e ]       = cls_top; // current point to new node
            ca[ cls_top ] = cls_top; // make new node as root
            e_rt = cls_top ++;       // e_rt is the new node index
        } else {              //[ middle node or root node  ]
            ca[ e ] = e_rt;          // e_rt already point to root, ensure element pointed to root.
        }

        // --------------------------------------------------------------------
        // solve all elements in a row for 'e'
        // --------------------------------------------------------------------
        for (
            cint_t n = e + 1, c_triu_pos = e_triu_pos + 1;
            n < e_num;
            n ++, c_triu_pos ++
        ) {
            if ( dist_triu[ c_triu_pos ] > dist_thresh ) { continue; }
            cint_t n_rt = n; for ( ; ca[ n_rt ] != n_rt; n_rt = ca[ n_rt ] ) { }
            ca[ n_rt ] = e_rt;  // make n's root belong to e's root
            ca[ n ]    = e_rt;  // make n belong to e's root
        }
    }

    // ------------------------------------------------------------------------
    //                            fixing root stage
    // ensure all elements point to root node
    // ------------------------------------------------------------------------
    for ( cint_t e = 0; e < e_num; e ++ ) {
        cint_t e_rt = e; for (; ca[ e_rt ] != e_rt; e_rt = ca[ e_rt ] ) { }
        if ( e_rt == ca[ e ] ) { continue; } // check if 'e' is already point to root index
        ca[ e ] = e_rt;                      // make sure 'e' is point to root
    }

    // ------------------------------------------------------------------------
    //                            final class ID stage
    // label the root node a class ID
    // ------------------------------------------------------------------------
    cint_t  cid = 0; // class id counter
    for ( cint_t e = e_num; e < cls_top; e ++ ) {
        if ( ca[ e ] == e ) { ca[ e ] = cid ++; } // if node point to self, means a root node
    }

    return ( int )( cid );
}




// ============================================================================
// generate the class table from class array
// ============================================================================
QXPACK_IC_HIDDEN int  QxPack_IcNdcThrTblFromAry (
    void *clas_tbl, int clas_tbl_sz, const void *clas_ary, int clas_ary_sz
) {
    // ------------------------------------------------------------------------
    //                            class table memory map
    // [ class list entry table ][ object id list area  ]
    // ------------------------------------------------------------------------
    // ------------------------------------------------------------------------
    // we need a large memory to record and store the temporary variables
    // ------------------------------------------------------------------------
    if ( clas_ary == NULL || clas_ary_sz < 1 ) { return -1; }

    cint_t dim = ( cint_t )( gDimByClasSize( clas_ary_sz ) );
    int clas_req_sz = dim * 4 * sizeof( cint_t );
    if ( clas_tbl == NULL || clas_tbl_sz < 1 )                { return clas_req_sz; }
    if ( clas_tbl_sz < clas_req_sz )                          { return -2; }
    if ( (( uintptr_t )( clas_tbl ) % sizeof(cint_t )) != 0 ) { return -9; } // invalid address...

    // ------------------------------------------------------------------------
    //                            initialize stage
    // ------------------------------------------------------------------------
    cint_t *ca  = & (( cint_t *) clas_ary )[0];
    cint_t *ent = & (( cint_t *) clas_tbl )[0];
    cint_t *lnk = & (( cint_t *) clas_tbl )[ dim + dim ];  // [ object id list area ]
    for ( cint_t i = 0, s = 0; i < dim; i ++, s += 2 ) {
        ent[s] = 0; ent[s+1] = -1; lnk[s] = -1; lnk[s+1] = -1;
    }

    // ------------------------------------------------------------------------
    //                            process stage
    // this routine scan the [ obj node area ] of clas_ary
    // ------------------------------------------------------------------------
    cint_t lnk_top = 0;
    for ( cint_t e = 0; e < dim; e ++ ) {
        cint_t cid   = ca[ e ] - dim - 1;  // cid is the index about current class
        cint_t c_ofv = cid + cid;                // correct index about
        cint_t pv    = ent[ c_ofv + 1 ];         // pv point to next object

        cint_t t_ofv = lnk_top + lnk_top;        // use current node
        lnk[ t_ofv ] = e; lnk[ t_ofv + 1 ] = pv; // fill pair ( object id, next )

        ++ ent[ c_ofv ];                         // increase class element count
        ent[ c_ofv + 1 ] = lnk_top ++;           // lnk_top means a class id, fetch it to new class id
    }

    // ------------------------------------------------------------------------
    //                            final stage
    // make the valid ent item continue
    // ------------------------------------------------------------------------
    cint_t r_ofv = 0;
    for ( cint_t i = 0, e_ofv = 0; i < dim; i ++, e_ofv += 2 ) {
        if ( ent[ e_ofv ] > 0 ) {
            ent[ r_ofv ] = ent[ e_ofv ]; ent[ r_ofv + 1 ] = ent[ e_ofv + 1 ];
            if ( e_ofv > r_ofv ) { ent[ e_ofv ] = 0; ent[ e_ofv + 1 ] = -1; } // mark it invalid
            r_ofv += 2;
        }
    }

    return ( int )( r_ofv / 2 );  // the total class number
}



#endif
