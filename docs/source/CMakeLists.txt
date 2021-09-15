find_program(M2R NAMES m2r m2r-3)

function(DefaultRstPath MDFILE RSTFILE_VAR)
    string(REGEX REPLACE "\\.md$" ".rst" FILEPATH ${MDFILE})
    get_filename_component(FILENAME ${FILEPATH} NAME)
    set(${RSTFILE_VAR} "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}" PARENT_SCOPE)
endfunction()

function(UpdateRstFromMd RSTFILE MDFILE)
    if(${MDFILE} IS_NEWER_THAN ${RSTFILE})
        message("--   Processing ${MDFILE} ...")
        execute_process(
                COMMAND ${M2R} --overwrite --parse-relative-links ${MDFILE}
                RESULTS_VARIABLE M2R_STATUS
        )
        if(M2R_STATUS)
            message("     Failed to update ${RSTFILE}: ${M2R_STATUS}")
        else()
            string(REGEX REPLACE "\\.md$" ".rst" FILEPATH ${MDFILE})
            file(RENAME ${FILEPATH} ${RSTFILE} )
            message("     Updated ${RSTFILE}: OK")
        endif()
    endif()
endfunction()

if(M2R)
    message("-- Checking ChangeLogs, BUGS, etc...")
    file(GLOB CHANGELOGS "${CMAKE_SOURCE_DIR}/ChangeLog-*.md")
    foreach(CHANGELOG IN LISTS CHANGELOGS)
        DefaultRstPath(${CHANGELOG} RESTFILE)
        UpdateRstFromMd(${RESTFILE} ${CHANGELOG})
    endforeach()
    UpdateRstFromMd(issues.rst "${CMAKE_SOURCE_DIR}/BUGS.md")
endif()
