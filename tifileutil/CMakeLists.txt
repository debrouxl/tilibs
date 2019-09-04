cmake_minimum_required(VERSION 3.12)

project(tifileutil
        VERSION     0.1
        LANGUAGES   CXX)

set(HEADER_FILES
        src/gettext.h
        src/logging.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/main.cc)

# external deps lookup
find_package(LibArchive REQUIRED)
pkg_check_modules(glib REQUIRED IMPORTED_TARGET glib-2.0)

add_executable(tifileutil ${SRC_FILES})

target_compile_definitions(tifileutil PRIVATE PACKAGE="TIFILEUTIL" PACKAGE_VERSION="${PROJECT_VERSION}")

target_include_directories(tifileutil PRIVATE src)

if(USE_ICONV)
    find_package(Iconv REQUIRED)
    add_compile_definitions(USE_ICONV)
    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(tifileutil PRIVATE ${Iconv_INCLUDE_DIRS})
        target_link_libraries(tifileutil PRIVATE ${Iconv_LIBRARIES})
    endif()
endif()

# internal deps
target_include_directories(tifileutil PRIVATE
        ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
        ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src)

add_dependencies(tifileutil ticonv tifiles2)
target_link_directories(tifileutil PRIVATE
        ${CMAKE_BINARY_DIR}/libticonv/trunk
        ${CMAKE_BINARY_DIR}/libtifiles/trunk)
target_link_libraries(tifileutil PRIVATE PkgConfig::glib LibArchive::LibArchive ticonv tifiles2)

# Takes care of the i18n po/pot/gmo/mo files
if(ENABLE_NLS)
    i18n_mo_from_po_pot()
    if(TARGET potfiles_1)
        add_dependencies(tifileutil potfiles_1)
    else()
        message(WARNING "tifileutil target potfiles_1 (i18n) is not available - did you run intltool-update?")
        remove_definitions(-DENABLE_NLS)
    endif()
endif()

# Install rules
install(TARGETS tifileutil
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE  DESTINATION ${CMAKE_INSTALL_BINDIR})
