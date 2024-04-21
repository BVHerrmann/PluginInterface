set ( Eigen3_DEFAULT_VERSION 3.3.9 )

if (TARGET Eigen3::Eigen)
  # check if version requirements are matched
else ()
  include(ExternalProject)
  
  if ( Eigen3_FIND_VERSION )
    set ( Eigen3_VERSION ${Eigen3_FIND_VERSION} )
  else()
    set ( Eigen3_VERSION ${Eigen3_DEFAULT_VERSION} )
  endif()
  
  ExternalProject_Add(Eigen3_download
	  EXCLUDE_FROM_ALL TRUE
    URL "https://gitlab.com/libeigen/eigen/-/archive/${Eigen3_VERSION}/eigen-${Eigen3_VERSION}.zip"
    # CMAKE_ARGS ${ep_common_args} -DCMAKE_INSTALL_PREFIX=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -DINCLUDE_INSTALL_DIR=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/include
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  # for remote debugging
  set_target_properties ( Eigen3_download PROPERTIES VS_CONFIGURATION_TYPE StaticLibrary )
  
  # create Eigen target to include
  add_library( Eigen3::Eigen INTERFACE IMPORTED )
  add_dependencies( Eigen3::Eigen Eigen3_download )

  ExternalProject_Get_Property(Eigen3_download SOURCE_DIR)
  set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${SOURCE_DIR})
  
  set ( Eigen3_INCLUDE_DIR ${SOURCE_DIR} CACHE PATH "Eigen3 Include Directory" )
  set ( Eigen3_FOUND TRUE )
  set ( Eigen3_Version ${Eigen3_VERSION} )
  message( STATUS "Eigen3 version: ${Eigen3_VERSION}" )
endif()
