function(enable_dtrace_for TARGET PROBES_D PROBES_H)
    if(HAVE_DTRACE AND NOT CMAKE_CROSSCOMPILING)
        target_sources(${TARGET} PRIVATE ${PROBES_H})
        add_custom_command(
                OUTPUT
                    ${PROBES_H}
                MAIN_DEPENDENCY
                    ${PROBES_D}
                COMMAND
                    ${DTRACE_EXECUTABLE}
                ARGS
                    -x nolibs -h
                    -s ${CMAKE_CURRENT_SOURCE_DIR}/${PROBES_D}
                    -o ${PROBES_H}
        )
        if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
            add_custom_command(
                    OUTPUT
                        ${TARGET}_probes.o
                    MAIN_DEPENDENCY
                        ${PROBES_H}
                    COMMAND
                        ${DTRACE_EXECUTABLE}
                    ARGS
                        -x nolibs -G
                        -s ${CMAKE_CURRENT_SOURCE_DIR}/${PROBES_D}
                        -o ${TARGET}_probes.o
            )
            target_sources(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}_probes.o)
            set_source_files_properties(${TARGET}_probes.o PROPERTIES
                    GENERATED true
                    EXTERNAL_OBJECT true)
        else() # not Linux
            set(PROBES_C ${TARGET}_probes.cc)
            file(GENERATE
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${PROBES_C}
                    CONTENT "#include \"${PROBES_H}\""
                    )
            add_custom_command(
                    TARGET ${TARGET}
                        PRE_LINK
                    DEPENDS
                        ${PROBES_H}
                    COMMAND
                        rm -f ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TARGET}.dir/${PROBES_C}.o
                    COMMAND
                        ${DTRACE_EXECUTABLE} -x nolibs -G
                        -s ${CMAKE_CURRENT_SOURCE_DIR}/${PROBES_D}
                        -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TARGET}.dir/${PROBES_C}.o
                           ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TARGET}.dir/*.o
                )
            target_sources(${TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${PROBES_C})
        endif()
    endif()
endfunction()
