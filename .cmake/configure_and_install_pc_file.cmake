function(configure_and_install_pc_file name version)
    set(prefix      "${CMAKE_INSTALL_PREFIX}")
    set(exec_prefix "\${prefix}")
    set(libdir      "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set(includedir  "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}")
    set(VERSION     "${version}")

    configure_file(${name}.pc.in ${PROJECT_BINARY_DIR}/${name}.pc @ONLY)
    set(ENV{PKG_CONFIG_PATH} "${PROJECT_BINARY_DIR}:$ENV{PKG_CONFIG_PATH}")

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.pc DESTINATION "${libdir}/pkgconfig")
endfunction()
