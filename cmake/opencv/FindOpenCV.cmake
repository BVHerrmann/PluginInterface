set ( OpenCV_DEFAULT_VERSION 4.5.2 )

if ( TARGET OpenCV )
  # check if version requirements are matched
  if ( OpenCV_FIND_VERSION AND OpenCV_Version VERSION_LESS OpenCV_FIND_VERSION )
      message( FATAL_ERROR "Found OpenCV Version ${OpenCV_Version} does not meet the minimum requirement of version ${OpenCV_FIND_VERSION}" )
  endif()
else ()
  include( ExternalProject )
  find_package( Eigen3 )
  
  if ( OpenCV_FIND_VERSION )
    set ( OpenCV_VERSION ${OpenCV_FIND_VERSION} )
  else()
    set ( OpenCV_VERSION ${OpenCV_DEFAULT_VERSION} )
  endif()
  
  ExternalProject_Add(OpenCV_build
  	EXCLUDE_FROM_ALL TRUE
  	URL "https://codeload.github.com/Itseez/opencv/zip/${OpenCV_VERSION}"
  	DOWNLOAD_NAME opencv.zip
  	CMAKE_GENERATOR ${gen}
  	CMAKE_ARGS
  		${ep_common_args}
  		-DBUILD_DOCS:BOOL=OFF
  		-DBUILD_EXAMPLES:BOOL=OFF
  		-DBUILD_PACKAGE:BOOL=OFF
  		-DBUILD_PERF_TESTS:BOOL=OFF
  		-DBUILD_SHARED_LIBS:BOOL=ON
  		-DBUILD_TESTS:BOOL=OFF
	
  		-DBUILD_opencv_apps:BOOL=OFF
  		-DBUILD_opencv_python2:BOOL=OFF
      -DBUILD_opencv_python3:BOOL=OFF
  		-DBUILD_opencv_ts:BOOL=OFF
  		-DBUILD_opencv_world:BOOL=ON
        
  		-DINSTALL_TESTS:BOOL=OFF
	
  		-DWITH_CUDA:BOOL=OFF
  		-DWITH_FFMPEG:BOOL=OFF
  		-DWITH_IPP:BOOL=OFF
  		-DWITH_MATLAB:BOOL=OFF
  		-DWITH_PVAPI:BOOL=OFF
      -DWITH_PROTOBUF:BOOL=OFF
		
  		-DOPENCV_BIN_INSTALL_PATH=${CMAKE_BINARY_DIR}/$<CONFIGURATION>
  		-DOPENCV_LIB_INSTALL_PATH=${CMAKE_BINARY_DIR}/$<CONFIGURATION>
  		-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/$<CONFIGURATION>
  )

  # for remote debugging
  set_target_properties ( OpenCV_build PROPERTIES VS_CONFIGURATION_TYPE StaticLibrary )

  if ( WIN32 )
  	string( REPLACE "." "" OPENCV_VERSION_CLEAN ${OpenCV_VERSION} )
  endif ( )

  add_dependencies( OpenCV_build Eigen3::Eigen )

  # create OpenCV target to include
  add_library( OpenCV SHARED IMPORTED )
  add_dependencies( OpenCV OpenCV_build )

  # make sure ALL include directories exist set INTERFACE_INCLUDE_DIRECTORIES throws error otherwise
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/opencv4" )
  foreach ( _CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} ) 
  	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${_CONFIG_TYPE}/include/opencv4" )
  endforeach()
  
  set ( OpenCV_INCLUDE_DIR "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/include" CACHE PATH "OpenCV Include Directory" )
  target_include_directories(OpenCV INTERFACE
	"${OpenCV_INCLUDE_DIR}"
    "${OpenCV_INCLUDE_DIR}/opencv4"
	"${CMAKE_BINARY_DIR}"
	)

  if ( APPLE )
    set_target_properties(OpenCV PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/libopencv_world.dylib")
  else ( )
    set_target_properties(OpenCV PROPERTIES
		IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/opencv_world${OPENCV_VERSION_CLEAN}.dll"
		IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/opencv_world${OPENCV_VERSION_CLEAN}d.dll"
		IMPORTED_IMPLIB "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/opencv_world${OPENCV_VERSION_CLEAN}.lib"
		IMPORTED_IMPLIB_DEBUG "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/opencv_world${OPENCV_VERSION_CLEAN}d.lib"
	)
  endif ( )
  
  set ( OpenCV_FOUND TRUE )
  set ( OpenCV_Version ${OpenCV_VERSION} )
  message( STATUS "OpenCV version: ${OpenCV_VERSION}" )
endif()
