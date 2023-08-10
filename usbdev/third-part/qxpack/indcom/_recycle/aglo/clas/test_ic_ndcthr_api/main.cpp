#include <stdio.h>
#include <stdlib.h>

#include "../qxpack_ic_ndcthr_api.h"

// ============================================================================
// output clas ary information
// ============================================================================
static void  prn_ndcThrAry( const void *ary, int ary_sz )
{
    int  dim = ary_sz / 2/ sizeof(int);
    const int *ca  = reinterpret_cast<const int*>(ary);

    printf("=== class array information ===\n");
    printf(": the content area\n");
    for ( int i = 0; i < dim; i ++ ) { printf("%2d,", ca[i]); }
    printf("\n");

    printf(": the class ID area\n");
    for ( int i = dim; i < dim+dim; i ++ ) { printf("%2d,", ca[i] );}
    printf("\n");

    printf(": object list, from 'a' to 'z'\n");
    for ( int i = 0; i < dim; i ++ ) {
        printf("'%c'->(%d) \n", i+'a', ca[ ca[i] ] );
    }
    printf("\n");
}

// ============================================================================
// output clas tbl information
// ============================================================================
static void  prn_ndcThrTbl ( const void *tb, int tbl_sz )
{
    int  dim = tbl_sz / 4 / sizeof(int);
    const int *tbl = reinterpret_cast<const int*>(tb);

    printf("=== class table information ===\n");
    printf(": the entry area\n");
    for ( int i = 0; i < dim; i ++ ) {
        printf("(%2d,%2d) ", tbl[i*2], tbl[i*2+1]);
    }
    printf("\n");
    printf(": the list area\n");
    const int *lnk = tbl + dim * 2;
    for ( int i = 0; i < dim; i ++ ) {
        if ( lnk[ i*2 ] < 0 ) { continue; }
        printf("(%c,%2d)\n", lnk[i*2] + 'a', lnk[i*2+1]);
    }
    printf("\n");

    printf(": the class object list\n");
    for ( int i = 0; tbl[i*2] > 0  && i < dim; i ++ ) {
        printf(" class %d: ", i);
        int next = tbl[i*2+1];
        for(; next >= 0;) {
            printf("%c,", lnk[next*2] + 'a');
            next = lnk[next*2+1];
        }
    }
    printf("\n");
}

// ============================================================================
// test clas array generator
// ============================================================================
static void  test_clasAryGen( )
{
    //float triu[] = { 0,3,2,1,4,  0,1,2,1,  0,1,2, 0,3, 0 };
    float triu[] = { 0,5,2,3,1,  0,2,1,3,  0,1,1,  0,2, 0 };

    int ary_sz   = QxPack_IcNdcThrAryFromTriu( nullptr, 0, triu, 5, 1 );
    void *ary    = malloc( ary_sz );
    int cls_num  = QxPack_IcNdcThrAryFromTriu( ary, ary_sz, triu, 5, 1 );
    prn_ndcThrAry( ary, ary_sz );

    free( ary );
}

// ============================================================================
// test class table generator
// ============================================================================
static void  test_clasTblGen( )
{
    //float triu[] = { 0,3,2,1,4,  0,1,2,1,  0,1,2, 0,3, 0 };
    float triu[] = { 0,5,2,3,1,  0,2,1,3,  0,1,1,  0,2, 0 };
    int ary_sz   = QxPack_IcNdcThrAryFromTriu( nullptr, 0, triu, 5, 1 );
    void *ary    = malloc( ary_sz );
    int cls_num  = QxPack_IcNdcThrAryFromTriu( ary, ary_sz, triu, 5, 1 );

    int tbl_sz   = QxPack_IcNdcThrTblFromAry( nullptr, 0, ary, ary_sz );
    void *tbl    = malloc( tbl_sz );
    int cls_num2 = QxPack_IcNdcThrTblFromAry( tbl, tbl_sz, ary, ary_sz );
    prn_ndcThrTbl( tbl, tbl_sz );

    free( tbl ); free( ary );
}


// ////////////////////////////////////////////////////////////////////////////
// entry
// ////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    (void)(argc); (void)(argv);
    test_clasAryGen();
    test_clasTblGen();
    return 0;
}
