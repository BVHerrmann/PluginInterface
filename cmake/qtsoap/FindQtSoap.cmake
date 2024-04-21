include(ExternalProject)

if (TARGET QtSoap)

else ()
  find_package( Qt5 COMPONENTS Core Widgets Network )
  
  add_library ( QtSoap STATIC "${CMAKE_CURRENT_LIST_DIR}/qtsoap/src/qtsoap.h" "${CMAKE_CURRENT_LIST_DIR}/qtsoap/src/qtsoap.cpp" )
  #target_compile_definitions( QtSoap PRIVATE
  #  _UNICODE
  #  UNICODE
  #)
  set_target_properties( QtSoap PROPERTIES FOLDER "ExternalProjectTargets")
  target_link_libraries( QtSoap Qt5::Core Qt5::Widgets Qt5::Network Qt5::Xml )
    
  target_include_directories( QtSoap INTERFACE "${CMAKE_CURRENT_LIST_DIR}/qtsoap/src/" )
  
  message( STATUS "QtSoap" )
endif()
