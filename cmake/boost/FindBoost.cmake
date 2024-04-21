set ( Boost_DEFAULT_VERSION 1.76.0 )

include(ExternalProject)

if (TARGET Boost::Boost)
  # check if version requirements are matched
  if ( Boost_FIND_VERSION )
    if ( Boost_FIND_VERSION_EXACT AND NOT Boost_VERSION VERSION_EQUAL Boost_FIND_VERSION )
      message( "Version ${Boost_VERSION} does not match requested version ${Boost_FIND_VERSION}" )
    elseif ( Boost_VERSION VERSION_LESS Boost_FIND_VERSION )
      message( "Version ${Boost_VERSION} does not match minimum requested version ${Boost_FIND_VERSION}" )
    endif()
  endif()
else ()
  if ( Boost_FIND_VERSION_EXACT AND Boost_FIND_VERSION )
    set ( Boost_VERSION ${Boost_FIND_VERSION} )
  elseif ( Boost_FIND_VERSION VERSION_GREATER Boost_DEFAULT_VERSION )
    message( WARNING "Untested Version ${Boost_FIND_VERSION} requested" )
    set ( Boost_VERSION ${Boost_FIND_VERSION} )
  else ()
    set ( Boost_VERSION ${Boost_DEFAULT_VERSION} )
  endif()
  
  string( REPLACE "." "_" Boost_VERSION_UNDERSCORE ${Boost_VERSION} )
  
  ExternalProject_Add(Boost_download
    EXCLUDE_FROM_ALL TRUE
    URL "https://boostorg.jfrog.io/artifactory/main/release/${Boost_VERSION}/source/boost_${Boost_VERSION_UNDERSCORE}.zip"
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
  )

  # make sure only headers will be used
  add_definitions( -DBOOST_ALL_NO_LIB=1 )

  # create Boost target to include
  add_library( Boost::Boost INTERFACE IMPORTED )
  add_dependencies( Boost::Boost Boost_download )
  
  ExternalProject_Get_Property(Boost_download SOURCE_DIR)
  set_target_properties(Boost::Boost PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${SOURCE_DIR})
  
  set ( Boost-INCLUDE_DIR ${SOURCE_DIR} CACHE PATH "Boost Include Directory" )
  set ( Boost_FOUND TRUE )
  set ( Boost_Version ${Boost_VERSION} )
  message( STATUS "Boost version: ${Boost_VERSION}" )
endif()