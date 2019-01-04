function(create_targets_both_lib_types basename)
    set(lib_objlib ${basename}_objlib)
    set(lib_shared ${basename}_shared)
    set(lib_static ${basename}_static)

    # Object target to unify source building for both (shared/static) real targets
    add_library(${lib_objlib} OBJECT ${SRC_FILES})
    set_target_properties(${lib_objlib} PROPERTIES POSITION_INDEPENDENT_CODE 1)

    # The two real targets
    add_library(${lib_shared} SHARED $<TARGET_OBJECTS:${lib_objlib}>)
    add_library(${lib_static} STATIC $<TARGET_OBJECTS:${lib_objlib}>)

    # Internal deps
    foreach(idep ${ARGN})
        add_dependencies(${lib_shared} ${idep}_shared)
        add_dependencies(${lib_static} ${idep}_static)
    endforeach()

    # Main properties
    set_target_properties(${lib_shared} ${lib_static} PROPERTIES
        OUTPUT_NAME     ${basename}
        PUBLIC_HEADER  "${PUBLIC_HEADERS}")

    # Defines
    target_compile_definitions(${lib_objlib} PRIVATE PACKAGE="${PROJECT_NAME}" VERSION="${PROJECT_VERSION}")

    # CFLAGS and include dirs
    target_compile_options(${lib_objlib} PRIVATE ${DEPS_CFLAGS})
    target_include_directories(${lib_objlib} PRIVATE src)

    # Link-related properties, flags...
    link_directories(${DEPS_LIBRARY_DIRS})
    target_link_libraries(${lib_shared} "${DEPS_LDFLAGS}" ${DEPS_LIBRARIES} ${Intl_LIBRARIES})

    # Stuff to install and developer-related things
    install(TARGETS ${lib_shared} ${lib_static}
        ARCHIVE         DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY         DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME         DESTINATION ${CMAKE_INSTALL_BINDIR}
        PUBLIC_HEADER   DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/tilp2)

    configure_and_install_pc_file(${basename} ${PROJECT_VERSION})

    create_buildandinstall_target(${PROJECT_NAME} ${lib_shared} ${lib_static})
endfunction()
