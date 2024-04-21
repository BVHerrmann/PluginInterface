function( add_translations TARGET TS_FILES )
    # combine arguments
    list( APPEND TS_FILES ${ARGN} )
    
    # create qm files
    qt5_add_translation( QM_FILES ${TS_FILES} )
    
    # create qrc file
    set( QRC "<RCC>\n\t<qresource prefix=\"/translations\">\n" )
    foreach( QM_FILE ${QM_FILES} )
        get_filename_component( QM_FILE_NAME ${QM_FILE} NAME)
        set( QRC "${QRC}\t\t<file>${QM_FILE_NAME}</file>\n" )    
    endforeach()
    set( QRC "${QRC}\t</qresource>\n</RCC>\n" )
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/translations_${TARGET}.qrc" ${QRC})
    
    # update target
    target_sources( ${TARGET}
        PRIVATE
            ${QM_FILES}
            ${CMAKE_CURRENT_BINARY_DIR}/translations_${TARGET}.qrc )
    
    # ts file update target 
    add_custom_target( "${TARGET}_lupdate"
       WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
    set_target_properties( "${TARGET}_lupdate" PROPERTIES
        FOLDER "CMakeScripts"
        XCODE_GENERATE_SCHEME OFF )
    add_dependencies( lupdate "${TARGET}_lupdate" )
    foreach(_ts_file ${TS_FILES})
        add_custom_command(TARGET "${TARGET}_lupdate"
            COMMAND ${Qt5_LUPDATE_EXECUTABLE}
            ARGS "${CMAKE_CURRENT_SOURCE_DIR}" -ts ${_ts_file})
    endforeach()
endfunction()

function( git_remote_exists GIT_URL OUTPUT_VAR )
    # check if remote exists locally
    execute_process( COMMAND ${GIT_EXECUTABLE} remote -v
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      RESULT_VARIABLE GIT_RESULT
      OUTPUT_VARIABLE GIT_OUTPUT
      ERROR_VARIABLE GIT_ERROR
      OUTPUT_STRIP_TRAILING_WHITESPACE )
    
    if( ${GIT_OUTPUT} MATCHES ".*${GIT_URL}.*" )
        set(${OUTPUT_VAR} 1 PARENT_SCOPE)
    else()
        # check remotely
        execute_process( COMMAND ${GIT_EXECUTABLE} ls-remote --exit-code -h ${GIT_URL}
          WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
          RESULT_VARIABLE GIT_RESULT
          OUTPUT_VARIABLE GIT_OUTPUT
          ERROR_VARIABLE GIT_ERROR
          OUTPUT_STRIP_TRAILING_WHITESPACE )
     
        if ( ${GIT_RESULT} EQUAL 0 )
            # remote exists
            set(${OUTPUT_VAR} 1 PARENT_SCOPE)
        
            # add remote for faster runtime next time
            message( "adding remote for ${GIT_URL}" )
            get_filename_component( _REMOTE_NAME ${GIT_URL} NAME_WE )
            execute_process( COMMAND ${GIT_EXECUTABLE} remote add ${_REMOTE_NAME} ${GIT_URL}
              WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
        else()
            # remote does not exist
            set(${OUTPUT_VAR} 0 PARENT_SCOPE)
        endif()
    endif()
endfunction()

function( add_subtree_actions NAME PREFIX REMOTE )
    
    git_remote_exists( ${REMOTE} GIT_REMOTE_EXISTS )
    if( ${GIT_REMOTE_EXISTS} )
        configure_file(
    	    ${CMAKE_SOURCE_DIR}/cmake/common/git_subtree_pull.cmake.in
            ${CMAKE_BINARY_DIR}/PULL_${NAME}.cmake
            @ONLY )
        add_custom_target( "${NAME}_subtree_pull"
            COMMAND cmake -P ${CMAKE_BINARY_DIR}/PULL_${NAME}.cmake
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
        set_target_properties( "${NAME}_subtree_pull" PROPERTIES FOLDER "CMakeScripts")
        file( READ ${CMAKE_BINARY_DIR}/PULL_${NAME}.cmake _SCRIPT )
        file( APPEND ${CMAKE_BINARY_DIR}/ALL_PULL.cmake "${_SCRIPT}\n")

        configure_file(
            ${CMAKE_SOURCE_DIR}/cmake/common/git_subtree_push.cmake.in
            ${CMAKE_BINARY_DIR}/PUSH_${NAME}.cmake
            @ONLY )
        add_custom_target( "${NAME}_subtree_push"
            COMMAND cmake -P ${CMAKE_BINARY_DIR}/PUSH_${NAME}.cmake
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} )
        set_target_properties( "${NAME}_subtree_push" PROPERTIES FOLDER "CMakeScripts")
        file( READ ${CMAKE_BINARY_DIR}/PUSH_${NAME}.cmake _SCRIPT )
        file( APPEND ${CMAKE_BINARY_DIR}/ALL_PUSH.cmake "${_SCRIPT}\n")
    endif()
    
endfunction()
