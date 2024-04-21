# build all available plugins
file( GLOB _PLUGINS LIST_DIRECTORIES true plugins/* )
foreach( _PLUGIN_DIR ${_PLUGINS} )
  if( IS_DIRECTORY ${_PLUGIN_DIR} )
    if ( EXISTS ${_PLUGIN_DIR}/CMakeLists.txt )
      get_filename_component( _PLUGIN ${_PLUGIN_DIR} NAME )
      list( APPEND PLUGINS ${_PLUGIN} )
    endif( )
  endif( )
endforeach()

# include plugins
foreach ( PLUGIN ${PLUGINS} )
  message( STATUS "Plugin ${PLUGIN}" )
  add_subdirectory( plugins/${PLUGIN} )
	set_target_properties( ${PLUGIN} PROPERTIES FOLDER "Plugins")
  if ( WIN32 )
    set_target_properties( ${PLUGIN} PROPERTIES VERSION ${PROJECT_VERSION} )
  endif ( )
  
  # pull and push scripts
  add_subtree_actions( "${PLUGIN}-plugin" "plugins/${PLUGIN}" "git@github.com:BertramBildverarbeitung/plugin-${PLUGIN}.git" )
endforeach ( )