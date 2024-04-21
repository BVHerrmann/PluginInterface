set ( Qwt_DEFAULT_VERSION 6.1.6 )

include(ExternalProject)

if (TARGET Qwt)
  # check if version requirements are matched
  if ( Qwt_FIND_VERSION )
    if ( Qwt_FIND_VERSION_EXACT AND NOT Qwt_VERSION VERSION_EQUAL Qwt_FIND_VERSION )
      message( "Version ${Qwt_VERSION} does not match requested version ${Qwt_FIND_VERSION}" )
    elseif ( Qwt_VERSION VERSION_LESS Qwt_FIND_VERSION )
      message( "Version ${Qwt_VERSION} does not match minimum requested version ${Qwt_FIND_VERSION}" )
    endif()
  endif()
else ()
  if ( Qwt_FIND_VERSION_EXACT AND Qwt_FIND_VERSION )
    set ( Qwt_VERSION ${Qwt_FIND_VERSION} )
  elseif ( Qwt_FIND_VERSION VERSION_GREATER Qwt_DEFAULT_VERSION )
    message( WARNING "Untested Version ${Qwt_FIND_VERSION} requested" )
    set ( Qwt_VERSION ${Qwt_FIND_VERSION} )
  else ()
    set ( Qwt_VERSION ${Qwt_DEFAULT_VERSION} )
  endif()
  
  add_definitions( -DQWT_DLL )
  ExternalProject_Add(Qwt_build
    EXCLUDE_FROM_ALL TRUE
    URL "http://downloads.sourceforge.net/project/qwt/qwt/${Qwt_VERSION}/qwt-${Qwt_VERSION}.zip"
    PATCH_COMMAND ${CMAKE_COMMAND} -E
      copy ${CMAKE_CURRENT_LIST_DIR}/CMakeLists.qwt.txt
  	  <SOURCE_DIR>/CMakeLists.txt
    LOG_CONFIGURE 1
    LOG_BUILD 1
    CMAKE_ARGS ${ep_common_args} -DCMAKE_PREFIX_PATH=${Qt5_DIR}/../../../ -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -DINCLUDE_INSTALL_DIR=${CMAKE_BINARY_DIR}/$<CONFIGURATION>/include
  )
  
  # for remote debugging
  set_target_properties ( Qwt_build PROPERTIES VS_CONFIGURATION_TYPE StaticLibrary )

  # create Qwt target to include
  add_library( Qwt SHARED IMPORTED )
  add_dependencies( Qwt Qwt_build )
  
  # make sure ALL include directories exist set INTERFACE_INCLUDE_DIRECTORIES throws error otherwise
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include" )
  foreach ( _CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} ) 
  	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${_CONFIG_TYPE}/include" )
  endforeach()
  
  set_target_properties(Qwt PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/include")

  if ( APPLE )
    set_target_properties(Qwt PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/libqwt.dylib")
  elseif ( WIN32 )
    set_target_properties(Qwt PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/qwt.dll")
    set_target_properties(Qwt PROPERTIES IMPORTED_IMPLIB "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/qwt.lib")
  endif ( )
  
  set ( Qwt_FOUND TRUE )
  set ( Qwt_Version ${Qwt_VERSION} )
  message( STATUS "Qwt version: ${Qwt_VERSION}" )
endif()
