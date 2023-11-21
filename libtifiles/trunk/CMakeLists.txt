cmake_minimum_required(VERSION 3.12)

project(libtifiles2
        VERSION     1.1.8
        LANGUAGES   CXX)

file(GLOB HEADER_FILES src/*.h)
set(SRC_FILES
        ${HEADER_FILES}
        src/cert.cc
        src/comments.cc
        src/error.cc
        src/files8x.cc
        src/files9x.cc
        src/filesnsp.cc
        src/filesxx.cc
        src/filetypes.cc
        src/grouped.cc
        src/intelhex.cc
        src/misc.cc
        src/rwfile.cc
        src/tifiles.cc
        src/tigroup.cc
        src/type2str.cc
        src/types68k.cc
        src/types83p.cc
        src/typesnsp.cc
        src/typesoldz80.cc
        src/typesxx.cc
        src/ve_fp.cc)

set(PUBLIC_HEADERS
        src/tifiles.h
        src/export2.h
        src/files8x.h
        src/files9x.h
        src/types73.h
        src/types82.h
        src/types83.h
        src/types83p.h
        src/types84p.h
        src/types85.h
        src/types86.h
        src/types89.h
        src/types89t.h
        src/types92.h
        src/types92p.h
        src/typesnsp.h
        src/typesv2.h
        src/typesxx.h)

# external deps lookup
pkg_check_modules(DEPS REQUIRED glib-2.0 libarchive)
try_static_libs_if_needed()

# auto-creation of all targets with flags etc., alongside with internal deps
create_targets_both_lib_types(tifiles2 ticonv)

set_target_properties(tifiles2_shared PROPERTIES VERSION 11.0.2 SOVERSION 11)

# there are relocation issues when linking BZip2 and zlib on linux:
#   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbz2.a(bzlib.o): relocation R_X86_64_PC32 against symbol `BZ2_crc32Table'
#   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libz.a(deflate.o): relocation R_X86_64_PC32 against symbol `z_errmsg'
#                can not be used when making a shared object; recompile with -fPIC
if(TRY_STATIC_LIBS AND NOT LINUX)
    find_package(BZip2 REQUIRED) # Needed for some reason
    target_link_libraries(tifiles2_shared ${BZIP2_LIBRARIES})
endif()

# Takes care of the i18n po/pot/gmo/mo files
if(ENABLE_NLS)
    i18n_mo_from_po_pot()
    add_dependencies(tifiles2_objlib potfiles_1)
endif()

# public export define
target_compile_definitions(tifiles2_objlib PUBLIC TIFILES_EXPORTS)

# tests
add_subdirectory(tests)
