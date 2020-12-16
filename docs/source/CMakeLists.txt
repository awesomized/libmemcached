find_program(M2R NAMES m2r m2r-3)

if(M2R)
    file(GLOB CHANGELOGS "${CMAKE_SOURCE_DIR}/ChangeLog-*.md")
    message("-- Checking ChangeLogs ...")
    foreach(CHANGELOG IN LISTS CHANGELOGS)
        string(REGEX REPLACE "\\.md$" ".rst" FILEPATH ${CHANGELOG})
        get_filename_component(FILENAME ${FILEPATH} NAME)

        set(RESTFILE "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
        if(${CHANGELOG} IS_NEWER_THAN ${RESTFILE})
            message("--   Processing ${CHANGELOG} ...")
            execute_process(
                    COMMAND ${M2R} --overwrite --parse-relative-links ${CHANGELOG}
                    RESULTS_VARIABLE M2R_STATUS
            )
            if(M2R_STATUS)
                message("     Failed to update ${RESTFILE}: ${M2R_STATUS}")
            else()
                file(RENAME ${FILEPATH} ${RESTFILE} )
                message("     Updated ${RESTFILE}: OK")
            endif()
        endif()
    endforeach()
endif()
