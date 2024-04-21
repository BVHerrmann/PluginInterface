if (TARGET Breakpad)
  # check if version requirements are matched
elseif ( WIN32 )
  add_library ( Breakpad STATIC
	"${CMAKE_CURRENT_LIST_DIR}/breakpad/src/client/windows/crash_generation/crash_generation_client.cc"
	"${CMAKE_CURRENT_LIST_DIR}/breakpad/src/client/windows/handler/exception_handler.cc"
	"${CMAKE_CURRENT_LIST_DIR}/breakpad/src/common/windows/guid_string.cc"
	)
  set_target_properties( Breakpad PROPERTIES FOLDER "ExternalProjectTargets")

  target_compile_definitions( Breakpad PRIVATE
    _UNICODE
    UNICODE
  )

  target_include_directories( Breakpad INTERFACE "${CMAKE_CURRENT_LIST_DIR}/breakpad/src/" )
  target_include_directories( Breakpad PRIVATE "${CMAKE_CURRENT_LIST_DIR}/breakpad/src/" )

  set ( Breakpad_FOUND TRUE )
  message( STATUS "Breakpad" )
endif()
