set ( WINPCAP_DEFAULT_VERSION 4.1.3 )

if ( WIN32 )
	if (TARGET WINPCAP)
	  # check if version requirements are matched
	else()
	    if ( WINPCAP_FIND_VERSION )
	      set ( WINPCAP_VERSION ${WINPCAP_FIND_VERSION} )
	    else()
	      set ( WINPCAP_VERSION ${WINPCAP_DEFAULT_VERSION} )
	    endif()
    
	    # create WinPcap target to include
	    add_library( Packet STATIC IMPORTED )
	    set_target_properties( Packet PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include" )
	    #set_target_properties( Packet PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/Lib/x64/Packet.lib" )	
		set_target_properties( Packet PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/Lib/Packet.lib" )	

	    # create WinPcap target to include
	    add_library( WinPcap STATIC IMPORTED )
	    set_target_properties( WinPcap PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/Include" )
	    #set_target_properties( WinPcap PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/Lib/x64/wpcap.lib" )
		set_target_properties( WinPcap PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/Lib/wpcap.lib" )

	  message( STATUS "WinPcap version: ${WINPCAP_VERSION}" )
	endif ( )
endif ( )