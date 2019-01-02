macro(create_buildandinstall_target name)
    file(RELATIVE_PATH TAR_INS_NAME ${CMAKE_SOURCE_DIR} "${PROJECT_SOURCE_DIR}")
    add_custom_target(buildandinstall_${name}
        DEPENDS ${ARGN}
        COMMAND ${CMAKE_COMMAND} -P "${TAR_INS_NAME}/cmake_install.cmake" --config $<CONFIGURATION>
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endmacro()
