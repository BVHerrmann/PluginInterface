message( STATUS "inspector" )

# find all modules
file( GLOB _CMAKE_DIRS LIST_DIRECTORIES true cmake/* )
foreach( _CMAKE_DIR ${_CMAKE_DIRS} )
  if( IS_DIRECTORY ${_CMAKE_DIR} )
      file( GLOB CMAKE_FILES "${_CMAKE_DIR}/Find*.cmake" )
      foreach( _CMAKE_FILE ${CMAKE_FILES} )
        include( ${_CMAKE_FILE} )
      endforeach()
  endif( )
endforeach()  

include(lib)
include(plugins)

add_subdirectory( inspector )
