function(create_lib_target_with_deps lib)
    add_library(${lib} ${SRC_FILES})

    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT error)
    if(lto_supported)
        set_target_properties(${lib} PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE
            INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
            INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
        )
    else()
        message(STATUS "IPO/LTO not supported: <${error}>")
    endif()

    # Internal deps
    foreach(idep ${ARGN})
        add_dependencies(${lib} ${idep})
        if(${idep} MATCHES "^(ti[a-z]+)2$")
            set(INTERNAL_DEP_LIB_DIR ${PROJECT_BINARY_DIR}/../../lib${CMAKE_MATCH_1}/trunk)
            set(INTERNAL_DEP_INC_DIR ${PROJECT_SOURCE_DIR}/../../lib${CMAKE_MATCH_1}/trunk/src)
        else()
            set(INTERNAL_DEP_LIB_DIR ${PROJECT_BINARY_DIR}/../../lib${idep}/trunk)
            set(INTERNAL_DEP_INC_DIR ${PROJECT_SOURCE_DIR}/../../lib${idep}/trunk/src)
        endif()
        if(GEN_IS_MULTI_CONFIG)
            set(INTERNAL_DEP_LIB_DIR "${INTERNAL_DEP_LIB_DIR}/$<CONFIG>")
        endif()
        target_include_directories(${lib} PRIVATE ${INTERNAL_DEP_INC_DIR})
        if (BUILD_SHARED_LIBS)
            target_link_directories(${lib} PRIVATE "${INTERNAL_DEP_LIB_DIR}")
            target_link_libraries(${lib} PRIVATE ${idep})
        else()
            target_link_libraries(${lib} PRIVATE "${INTERNAL_DEP_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${idep}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        endif()
    endforeach()

    # Main properties
    set_target_properties(${lib} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

    # Defines
    target_compile_definitions(${lib} PRIVATE PACKAGE="${PROJECT_NAME}" VERSION="${PROJECT_VERSION}")

    # Stuff to install and developer-related things
    install(TARGETS ${lib}
        ARCHIVE         DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY         DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME         DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tilp2")

    configure_and_install_pc_file(${lib} ${PROJECT_VERSION})
endfunction()
