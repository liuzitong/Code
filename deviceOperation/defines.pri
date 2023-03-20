defineTest( genTempCopyProFile ) {
    pro_file      = $$4/$$1
    source_list   = $$2
    base_dir      = $$3
    install_dir   = $$4
    include_dir   = $$5

    ! exists( $$4 ) : mkpath( $$4 )  # make sure install dir existed

    cpy_src_file = "CPY_SRC_FILE += \\"
    cpy_tgt_path = "CPY_TGT_PATH += \\"
    cpy_tgt_file = "CPY_TGT_FILE += \\"

    for( file, source_list ) {
        fn1= $$absolute_path( $$clean_path( $$file))
        fn2= $$relative_path( $$fn1, $$base_dir )  # get the relative path base on basedir
        fn3= $$section( fn2, ../, -1, -1 )
        fn_file = $$section( fn3, /, -1, -1 )
        fn_path = $$section( fn3, /,  0, -2 )

        cpy_tgt_path += "    $${include_dir}/$${fn_path} \\"
        cpy_tgt_file += "    $${include_dir}/$${fn_path}/$${fn_file}  \\"
        cpy_src_file += "    $$fn1 \\"
    }
    cpy_src_file += " "
    cpy_tgt_path += " "
    cpy_tgt_file += " "

    write_file( $$pro_file, cpy_src_file )
    write_file( $$pro_file, cpy_tgt_path, append )
    write_file( $$pro_file, cpy_tgt_file, append )

    cpy_code_src = \
        "for ( tgt_path, CPY_TGT_PATH ) {" \
        "    ! exists( \$\$tgt_path ) : mkpath( \$\$tgt_path ) " \
        "}" \
        "tgt_cntr = "\
        "for ( src_file, CPY_SRC_FILE ) { " \
        "    tgt_file = \$\$member( CPY_TGT_FILE, \$\$size( tgt_cntr )) " \
        "    tgt_cntr += 1 "\
        "    exists( \$\$src_file ) { "\
        "       message( copy \$\$src_file ) "\
        "       win32 {"\
        "           cpy_tgt_file = \$\$replace( tgt_file, /, \\\\ )" \
        "           cpy_src_file = \$\$replace( src_file, /, \\\\ )" \
        "           system( \"copy \$\${cpy_src_file} \$\${cpy_tgt_file} /y \")" \
        "       } else {" \
        "           system( \"\\cp -rf \$\${src_file} \$\${tgt_file}\" )" \
        "       }" \
        "   }"\
        "}"

    write_file( $$pro_file, cpy_code_src, append )
}
