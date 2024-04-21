set ( OpenCVStatic_DEFAULT_VERSION 4.5.2 )

if ( TARGET OpenCVStatic )
  # check if version requirements are matched
  if ( OpenCVStatic_FIND_VERSION AND OpenCVStatic_Version VERSION_LESS OpenCVStatic_FIND_VERSION )
      message( FATAL_ERROR "Found OpenCV Version ${OpenCVStatic_Version} does not meet the minimum requirement of version ${OpenCVStatic_FIND_VERSION}" )
  endif()
else ()
  include( ExternalProject )
  
  if ( OpenCVStatic_FIND_VERSION )
    set ( OpenCVStatic_VERSION ${OpenCVStatic_FIND_VERSION} )
  else()
    set ( OpenCVStatic_VERSION ${OpenCVStatic_DEFAULT_VERSION} )
  endif()
  
  ExternalProject_Add(OpenCVStatic_build
  	EXCLUDE_FROM_ALL TRUE
  	URL "https://codeload.github.com/opencv/opencv/zip/${OpenCVStatic_VERSION}"
  	DOWNLOAD_NAME opencv.zip
  	CMAKE_GENERATOR ${gen}
  	CMAKE_ARGS
  		${ep_common_args}
  		-DBUILD_DOCS:BOOL=OFF
  		-DBUILD_EXAMPLES:BOOL=OFF
  		-DBUILD_PACKAGE:BOOL=OFF
  		-DBUILD_PERF_TESTS:BOOL=OFF
  		-DBUILD_SHARED_LIBS:BOOL=OFF
  		-DBUILD_TESTS:BOOL=OFF
		-DBUILD_WITH_STATIC_CRT:BOOL=ON
	
  		-DBUILD_opencv_apps:BOOL=OFF
  		-DBUILD_opencv_python2:BOOL=OFF
        -DBUILD_opencv_python3:BOOL=OFF
  		-DBUILD_opencv_ts:BOOL=OFF
  		-DBUILD_opencv_world:BOOL=ON
        
  		-DINSTALL_TESTS:BOOL=OFF
	
		-DWITH_ADE:BOOL=OFF
  		-DWITH_CUDA:BOOL=OFF
		-DWITH_DIRECTX:BOOL=OFF
  		-DWITH_FFMPEG:BOOL=OFF
  		-DWITH_IPP:BOOL=ON
		-DWITH_ITT:BOOL=OFF
		-DWITH_JASPER:BOOL=OFF
		-DWITH_JPEG:BOOL=OFF
  		-DWITH_MATLAB:BOOL=OFF
		-DWITH_MSMF:BOOL=OFF
		-DWITH_OPENCL:BOOL=OFF
		-DWITH_OPENCLAMDBLAS:BOOL=OFF
		-DWITH_OPENCLAMDFFT:BOOL=OFF
		-DWITH_OPENGL:BOOL=OFF
		-DWITH_OPENEXR:BOOL=OFF
		-DWITH_OPENJPEG:BOOL=OFF
		-DWITH_PNG:BOOL=OFF
  		-DWITH_PVAPI:BOOL=OFF
        -DWITH_PROTOBUF:BOOL=OFF
		-DWITH_TIFF:BOOL=OFF
		-DWITH_VFW:BOOL=OFF
		-DWITH_WEBP:BOOL=OFF
		-DWITH_QUIRC:BOOL=OFF
		
  		-DOPENCV_BIN_INSTALL_PATH=${CMAKE_BINARY_DIR}/$<CONFIGURATION>/OpenCVStatic
  		-DOPENCV_LIB_INSTALL_PATH=${CMAKE_BINARY_DIR}/$<CONFIGURATION>/OpenCVStatic
		-DOPENCV_LIB_ARCHIVE_INSTALL_PATH=${CMAKE_BINARY_DIR}/$<CONFIGURATION>/OpenCVStatic
		-DOPENCV_3P_LIB_INSTALL_PATH=${CMAKE_BINARY_DIR}/$<CONFIGURATION>/OpenCVStatic
  		-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/$<CONFIGURATION>/OpenCVStatic
		-DINSTALL_CREATE_DISTRIB:BOOL=ON
  )

  if ( WIN32 )
  	string( REPLACE "." "" OPENCV_VERSION_CLEAN ${OpenCVStatic_VERSION} )
  endif ( )
  
  # create OpenCV target to include
  add_library( OpenCVStatic STATIC IMPORTED GLOBAL )
  add_dependencies( OpenCVStatic OpenCVStatic_build )

  # make sure ALL include directories exist set INTERFACE_INCLUDE_DIRECTORIES throws error otherwise
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/OpenCVStatic/include/opencv4" )
  foreach ( _CONFIG_TYPE ${CMAKE_CONFIGURATION_TYPES} ) 
  	file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${_CONFIG_TYPE}/OpenCVStatic/include/opencv4" )
  endforeach()
  
set ( OpenCVStatic_INCLUDE_DIR "${CMAKE_BINARY_DIR}/$<CONFIGURATION>/OpenCVStatic/include" CACHE PATH "OpenCV Static Include Directory" )
target_include_directories(OpenCVStatic INTERFACE
	"${OpenCVStatic_INCLUDE_DIR}"
    "${OpenCVStatic_INCLUDE_DIR}/opencv4"
	"${CMAKE_BINARY_DIR}"
	)

  if ( APPLE )
    set_target_properties(OpenCVStatic PROPERTIES IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/OpenCVStatic/libopencv_world.a")
  else ( )
	set_property(TARGET OpenCVStatic PROPERTY INTERFACE_LINK_LIBRARIES
		"${CMAKE_BINARY_DIR}/$(CONFIGURATION)/OpenCVStatic/ippicvmt.lib"
		"${CMAKE_BINARY_DIR}/$(CONFIGURATION)/OpenCVStatic/ippiw$<$<CONFIG:Debug>:d>.lib"
		"${CMAKE_BINARY_DIR}/$(CONFIGURATION)/OpenCVStatic/zlib$<$<CONFIG:Debug>:d>.lib"
	)
	
    set_target_properties(OpenCVStatic PROPERTIES
		IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/OpenCVStatic/opencv_world${OPENCV_VERSION_CLEAN}.lib"
		IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/$(CONFIGURATION)/OpenCVStatic/opencv_world${OPENCV_VERSION_CLEAN}d.lib"
	)
  endif ( )
  
  set ( OpenCVStatic_FOUND TRUE )
  set ( OpenCVStatic_Version ${OpenCVStatic_VERSION} )
  message( STATUS "OpenCV static version: ${OpenCVStatic_VERSION}" )
endif()
