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
pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive)
add_executable(tifileutil ${SRC_FILES})

try_static_libs_if_needed()

# Defines
target_compile_definitions(tifileutil PRIVATE PACKAGE="TIFILEUTIL" PACKAGE_VERSION="${PROJECT_VERSION}")

# CFLAGS and include dirs
if(TRY_STATIC_LIBS)
    target_compile_options(tifileutil PRIVATE ${DEPS_STATIC_CFLAGS})
else()
    target_compile_options(tifileutil PRIVATE ${DEPS_CFLAGS})
endif()
target_include_directories(tifileutil PRIVATE src)

if(USE_ICONV)
    find_package(Iconv REQUIRED)
    # flags/link for external deps
    add_compile_definitions(USE_ICONV)
endif()

# internal deps
target_include_directories(tifileutil PRIVATE
        ${CMAKE_SOURCE_DIR}/libticonv/trunk/src
        ${CMAKE_SOURCE_DIR}/libtifiles/trunk/src)

# Link-related properties, flags...
if(TRY_STATIC_LIBS)
    add_dependencies(tifileutil ticonv_static tifiles2_static)
    target_link_directories(tifileutil PRIVATE ${TRY_STATIC_DEPS_LIBSDIRS})
    target_link_libraries(tifileutil ${TRY_STATIC_DEPS_LDFLAGS_OTHER} ${TRY_STATIC_DEPS_LIBS} ticonv_objlib tifiles2_objlib)
    if(Iconv_FOUND AND NOT Iconv_IS_BUILT_IN)
        target_include_directories(tifileutil PRIVATE ${Iconv_INCLUDE_DIRS})
        if(Iconv_LIBRARY_DIRS)
            target_link_directories(tifileutil PRIVATE ${Iconv_LIBRARY_DIRS})
        endif()
        target_link_libraries(tifileutil ${Iconv_LIBRARIES})
    endif()
    if(LINUX)
        target_link_libraries(tifileutil ${CMAKE_DL_LIBS} -static-libgcc -static-libstdc++)
    endif()
else()
    add_dependencies(tifileutil ticonv_shared tifiles2_shared)
    target_link_directories(tifileutil PRIVATE
            ${DEPS_LIBRARY_DIRS}
            ${CMAKE_BINARY_DIR}/libticonv/trunk
            ${CMAKE_BINARY_DIR}/libtifiles/trunk)
    target_link_libraries(tifileutil ${DEPS_LDFLAGS_OTHER} ${DEPS_LIBRARIES} ticonv_objlib tifiles2_objlib)
endif()

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
