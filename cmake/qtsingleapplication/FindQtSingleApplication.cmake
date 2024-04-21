set ( Eigen3_DEFAULT_VERSION 3.3.7 )

include(ExternalProject)

if (TARGET QtSingleApplication)

else ()
  find_package( Qt5 COMPONENTS Core Widgets Network )
  
  add_library ( QtSingleApplication STATIC "${CMAKE_CURRENT_LIST_DIR}/qt-solutions/qtsingleapplication/src/qtlocalpeer.cpp" "${CMAKE_CURRENT_LIST_DIR}/qt-solutions/qtsingleapplication/src/qtsingleapplication.cpp" )
  target_compile_definitions( QtSingleApplication PRIVATE
    _UNICODE
    UNICODE
  )
  set_target_properties( QtSingleApplication PROPERTIES FOLDER "ExternalProjectTargets")
  target_link_libraries( QtSingleApplication Qt5::Core Qt5::Widgets Qt5::Network )
    
  target_include_directories( QtSingleApplication INTERFACE "${CMAKE_CURRENT_LIST_DIR}/qt-solutions/qtsingleapplication/src/" )
  
  message( STATUS "QtSingleApplication" )
endif()
