if(APPLE OR UNIX)
    set(STATIC_LIB_EXT ".a")
elseif(WIN32 OR MINGW)
    set(STATIC_LIB_EXT ".lib")
endif()

if(NOT CMAKE_FIND_LIBRARY_PREFIXES)
    set(STATIC_LIB_PREFIX "")
else()
    list(GET CMAKE_FIND_LIBRARY_PREFIXES 0 STATIC_LIB_PREFIX)
endif()

macro(try_static_libs_if_needed)
    if (TRY_STATIC_LIBS)
        list(APPEND DEPS_LIBRARY_DIRS        "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
        list(APPEND DEPS_STATIC_LIBRARY_DIRS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
        if(APPLE)
            list(APPEND DEPS_LIBRARY_DIRS "/usr/lib" "${CMAKE_OSX_SYSROOT}/usr/lib" "/opt/homebrew/lib" "/usr/local/opt" "/usr/local/lib")
            list(APPEND DEPS_STATIC_LIBRARY_DIRS "/opt/homebrew/lib" "/usr/local/opt" "/usr/local/lib" "${CMAKE_OSX_SYSROOT}/usr/lib" )
        endif()
        list(REMOVE_DUPLICATES DEPS_STATIC_LIBRARIES)
        list(REMOVE_DUPLICATES DEPS_STATIC_LIBRARY_DIRS)

        foreach(LIB_NAME ${DEPS_STATIC_LIBRARIES})
            # there's a weird libarchive/zlib issue with static on linux right now:
            #   /usr/lib/x86_64-linux-gnu/libarchive.a(archive_read_support_filter_gzip.o): undefined reference to symbol 'inflateEnd'
            #   /lib/x86_64-linux-gnu/libz.so.1: error adding symbols: DSO missing from command line
            # there are relocation issues when linking BZip2, zlib, and libusb on linux:
            #   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libbz2.a(bzlib.o): relocation R_X86_64_PC32 against symbol `BZ2_crc32Table'
            #   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libz.a(deflate.o): relocation R_X86_64_PC32 against symbol `z_errmsg'
            #   /usr/bin/ld: /usr/lib/x86_64-linux-gnu/libusb-1.0.a(libusb_1_0_la-core.o): relocation R_X86_64_PC32 against symbol `stderr@@GLIBC_2.2.5'
            #                can not be used when making a shared object; recompile with -fPIC
            if(   (LIB_NAME STREQUAL "archive" AND CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
               OR (LIB_NAME MATCHES "^(bz2|z|usb-1.0)$" AND CMAKE_SYSTEM_NAME STREQUAL "Linux"))
                message(WARNING "[Linux] Skipping '${LIB_NAME}' static lib search due to a linking issue, will fallback on the shared lib")
                list(APPEND TRY_STATIC_DEPS_LIBRARIES "${LIB_NAME}")
                set(${LIB_NAME}_FOUND YES)
                continue()
            endif()

            foreach(LIB_DIR ${DEPS_STATIC_LIBRARY_DIRS})
                if(NOT EXISTS ${LIB_DIR})
                    continue()
                endif()
                set(LIB_FULLPATH "${LIB_DIR}/lib${LIB_NAME}-static${STATIC_LIB_EXT}")
                if(NOT EXISTS ${LIB_FULLPATH})
                    set(LIB_FULLPATH "${LIB_DIR}/${STATIC_LIB_PREFIX}${LIB_NAME}${STATIC_LIB_EXT}")
                endif()
                if(NOT EXISTS ${LIB_FULLPATH})
                    set(LIB_FULLPATH "${LIB_DIR}/lib${LIB_NAME}${STATIC_LIB_EXT}")
                endif()
                if(NOT EXISTS ${LIB_FULLPATH})
                    set(LIB_FULLPATH "${LIB_DIR}/${LIB_NAME_FULL}/lib/${STATIC_LIB_PREFIX}${LIB_NAME}-static${STATIC_LIB_EXT}")
                endif()
                if(NOT EXISTS ${LIB_FULLPATH})
                    set(LIB_FULLPATH "${LIB_DIR}/lib${LIB_NAME_FULL}/lib/${STATIC_LIB_PREFIX}${LIB_NAME}${STATIC_LIB_EXT}")
                endif()
                if(NOT EXISTS ${LIB_FULLPATH})
                    set(LIB_NAME_FULL ${LIB_NAME})
                    if(LIB_NAME_FULL STREQUAL "bz2")
                        set(LIB_NAME_FULL "bzip2")
                    elseif(LIB_NAME_FULL STREQUAL "z")
                        set(LIB_NAME_FULL "zlib")
                    endif()
                    set(LIB_FULLPATH "${LIB_DIR}/${LIB_NAME_FULL}/lib/${STATIC_LIB_PREFIX}${LIB_NAME}${STATIC_LIB_EXT}")
                endif()
                if(EXISTS ${LIB_FULLPATH})
                    message("[Info] Found static lib '${LIB_NAME}' for '${PROJECT_NAME}': ${LIB_FULLPATH}")
                    list(APPEND TRY_STATIC_DEPS_LIBRARIES ${LIB_FULLPATH})
                    set(${LIB_NAME}_FOUND YES)
                    break()
                endif()
            endforeach()
            if(NOT ${LIB_NAME}_FOUND)
                message("[Warning] Could not find static lib '${LIB_NAME}' for '${PROJECT_NAME}'. Adding it directly anyway!")
                list(APPEND TRY_STATIC_DEPS_LIBRARIES "${LIB_NAME}")
            endif()
        endforeach()
        string(REPLACE ";" " " DEPS_STATIC_LDFLAGS_OTHER "${DEPS_STATIC_LDFLAGS_OTHER}")
        list(APPEND TRY_STATIC_DEPS_LIBRARIES "${DEPS_STATIC_LDFLAGS_OTHER}")
        list(REMOVE_DUPLICATES TRY_STATIC_DEPS_LIBRARIES)
    endif()
endmacro()
