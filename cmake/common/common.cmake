cmake_minimum_required( VERSION 3.13 )

# allow MSVC Runtime Selection
cmake_policy(SET CMP0091 NEW)

# enable modern C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# for remote debugging
set(CMAKE_SUPPRESS_REGENERATION true)

# Windows Optimizations
if ( WIN32 )
	# Enable Debug Information according to http://www.wintellect.com/devcenter/jrobbins/correctly-creating-native-c-release-build-pdbs
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")

	add_definitions( -D_USE_MATH_DEFINES )
    
    # Use Multi Processor Builds
    add_compile_options( /MP )
endif()

# add all cmake subfolders to module path
file( GLOB _CMAKE_DIRS LIST_DIRECTORIES true cmake/* )
foreach( _CMAKE_DIR ${_CMAKE_DIRS} )
  if( IS_DIRECTORY ${_CMAKE_DIR} )
      list( APPEND CMAKE_DIRS ${_CMAKE_DIR} )
  endif( )
endforeach()  
set( CMAKE_MODULE_PATH ${CMAKE_DIRS} ${CMAKE_MODULE_PATH} )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION> )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION> )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION> )

string( TIMESTAMP YEAR "%Y" UTC )
set( PROJECT_VENDOR "Bertram Bildverarbeitung GmbH" )
set( PROJECT_COPYRIGHT "2011-${YEAR} Bertram Bildverarbeitung GmbH" )

string( TIMESTAMP _MAJOR_VERSION "%y" UTC )
string( TIMESTAMP _MINOR_VERSION "%m" UTC )
STRING( REGEX REPLACE "^0([0-9])" "\\1" _MINOR_VERSION ${_MINOR_VERSION} )

set_property( GLOBAL PROPERTY USE_FOLDERS ON)
set_property( GLOBAL PROPERTY AUTOGEN_TARGETS_FOLDER "CMake")

find_package( Git )
execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET )
execute_process( COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET )
execute_process( COMMAND ${GIT_EXECUTABLE} log --oneline
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE _GIT_LOG ERROR_QUIET )
if ( _GIT_LOG )
  string( REGEX REPLACE "^([^\n]*)\n" "#" _GIT_LOG ${_GIT_LOG} )
  string( LENGTH ${_GIT_LOG} _PATCH_VERSION )
else()
  set( _PATCH_VERSION 0 )
endif()
set( PROJECT_VERSION "${_MAJOR_VERSION}.${_MINOR_VERSION}.${_PATCH_VERSION}" )
message( "Version ${PROJECT_VERSION}" )

get_filename_component( PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME )
string( REPLACE " " "_" PROJECT_NAME ${PROJECT_NAME} )

# installer creation
include( package )

# include custom helper functions
include( functions )

# scripts
add_custom_target( "lupdate"
   WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ALL_PULL.cmake "")
add_custom_target( "ALL_PULL"
    COMMAND cmake -P ${CMAKE_CURRENT_BINARY_DIR}/ALL_PULL.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ALL_PUSH.cmake "")
add_custom_target( "ALL_PUSH"
    COMMAND cmake -P ${CMAKE_CURRENT_BINARY_DIR}/ALL_PUSH.cmake
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
foreach( _CMAKE_DIR ${CMAKE_DIRS} )
    get_filename_component( _CMAKE ${_CMAKE_DIR} NAME )
    add_subtree_actions( ${_CMAKE} "cmake/${_CMAKE}" "git@github.com:BertramBildverarbeitung/cmake-${_CMAKE}.git" )
endforeach()
