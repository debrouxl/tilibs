function(create_targets_both_lib_types basename)
    set(lib_objlib ${basename}_objlib)
    set(lib_shared ${basename}_shared)
    set(lib_static ${basename}_static)

    # Object target to unify source building for both (shared/static) real targets
    add_library(${lib_objlib} OBJECT ${SRC_FILES})

    # The two real targets
    add_library(${lib_shared} SHARED $<TARGET_OBJECTS:${lib_objlib}>)
    add_library(${lib_static} STATIC $<TARGET_OBJECTS:${lib_objlib}>)

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
        target_include_directories(${lib_objlib} PRIVATE ${INTERNAL_DEP_INC_DIR})
        if(TRY_STATIC_LIBS)
            target_link_libraries(${lib_shared} "${INTERNAL_DEP_LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${idep}${CMAKE_STATIC_LIBRARY_SUFFIX}")
        else()
            target_link_directories(${lib_shared} PRIVATE "${INTERNAL_DEP_LIB_DIR}")
            target_link_libraries(${lib_shared} "${idep}")
        endif()
    endforeach()

    # Main properties
    if(MSVC)
        set(static_lib_output_name "${basename}-static")
    else()
        set(static_lib_output_name "${basename}")
    endif()
    set_target_properties(${lib_shared} PROPERTIES
        OUTPUT_NAME     ${basename}
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")
    set_target_properties(${lib_static} PROPERTIES
        OUTPUT_NAME     ${static_lib_output_name}
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")

    # Defines
    target_compile_definitions(${lib_objlib} PRIVATE PACKAGE="${PROJECT_NAME}" VERSION="${PROJECT_VERSION}")

    # CFLAGS and include dirs
    if(TRY_STATIC_LIBS)
        target_compile_options(${lib_objlib} PRIVATE ${DEPS_STATIC_CFLAGS})
    else()
        target_compile_options(${lib_objlib} PRIVATE ${DEPS_CFLAGS})
    endif()
    target_include_directories(${lib_objlib} PRIVATE src)

    # Link-related properties, flags...
    if(TRY_STATIC_LIBS)
        target_link_directories(${lib_shared} PRIVATE ${TRY_STATIC_DEPS_LIBSDIRS})
        target_link_libraries(${lib_shared} ${TRY_STATIC_DEPS_LDFLAGS_OTHER} ${TRY_STATIC_DEPS_LIBS})
    else()
        target_link_directories(${lib_shared} PRIVATE ${DEPS_LIBRARY_DIRS})
        target_link_libraries(${lib_shared} ${DEPS_LDFLAGS_OTHER} ${DEPS_LIBRARIES})
    endif()

    # Stuff to install and developer-related things
    install(TARGETS ${lib_shared} ${lib_static}
        ARCHIVE         DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY         DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME         DESTINATION "${CMAKE_INSTALL_BINDIR}"
        PUBLIC_HEADER   DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/tilp2")

    configure_and_install_pc_file(${basename} ${PROJECT_VERSION})
endfunction()
