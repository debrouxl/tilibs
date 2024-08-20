function(create_targets_both_lib_types basename)
    set(lib_objlib ${basename}_objlib)
    set(lib_shared ${basename}_shared)
    set(lib_static ${basename}_static)

    if(MSVC)
        set(STATIC_LIB_NAME_PRESUFFIX "-static")
    else()
        set(STATIC_LIB_NAME_PRESUFFIX "")
    endif()

    # Object target to unify source building for both (shared/static) real targets
    add_library(${lib_objlib} OBJECT ${SRC_FILES})

    # The two real targets
    add_library(${lib_shared} SHARED $<TARGET_OBJECTS:${lib_objlib}>)
    add_library(${lib_static} STATIC $<TARGET_OBJECTS:${lib_objlib}>)

    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT error)
    if(lto_supported)
        set_target_properties(${lib_objlib} PROPERTIES
            INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE
            INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
            INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
        )
    else()
        message(STATUS "IPO/LTO not supported: <${error}>")
    endif()

    # Internal deps
    foreach(idep ${ARGN})
        add_dependencies(${lib_objlib} ${idep}_objlib)
        add_dependencies(${lib_shared} ${idep}_shared)
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
        target_include_directories(${lib_objlib} PRIVATE ${INTERNAL_DEP_INC_DIR})
        if (VCPKG_TARGET_TRIPLET MATCHES "-static")
            target_link_libraries(${lib_shared} PRIVATE "${INTERNAL_DEP_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${idep}${STATIC_LIB_NAME_PRESUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        else()
            target_link_directories(${lib_shared} PRIVATE "${INTERNAL_DEP_LIB_DIR}")
            target_link_libraries(${lib_shared} PRIVATE ${idep})
        endif()
    endforeach()

    # Main properties
    set_target_properties(${lib_shared} PROPERTIES
        OUTPUT_NAME     ${basename}
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")
    set_target_properties(${lib_static} PROPERTIES
        OUTPUT_NAME    "${basename}${STATIC_LIB_NAME_PRESUFFIX}"
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")

    # Defines
    target_compile_definitions(${lib_objlib} PRIVATE PACKAGE="${PROJECT_NAME}" VERSION="${PROJECT_VERSION}")

    # Stuff to install and developer-related things
    install(TARGETS ${lib_shared} ${lib_static}
        ARCHIVE         DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY         DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME         DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tilp2")

    configure_and_install_pc_file(${basename} ${PROJECT_VERSION})
endfunction()
