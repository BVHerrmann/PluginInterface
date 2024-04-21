cmake_minimum_required(VERSION 3.18)

add_definitions( -D QT_MESSAGELOGCONTEXT )
set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
#set( CMAKE_INCLUDE_CURRENT_DIR ON )

# unset custom module path
set( _BAK_CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} )
unset( CMAKE_MODULE_PATH )

set( _BAK_CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} )

if ( APPLE )
  file( GLOB _QT_DIRS LIST_DIRECTORIES true "$ENV{HOME}/Qt/*" )
else ( )
  file( GLOB _QT_DIRS LIST_DIRECTORIES true "C:/Qt/*" )
endif ( )

# Qt compiler detection
if ( CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set( QT_COMPILER "clang" )
elseif( MSVC_TOOLSET_VERSION EQUAL 141 )
    set( QT_COMPILER "msvc2017" )
elseif( MSVC_TOOLSET_VERSION EQUAL 142 )
    set( QT_COMPILER "msvc2019" )
else()
    message(WARNING "Unknown Qt compiler for ${CMAKE_CXX_COMPILER_ID} ${MSVC_TOOLSET_VERSION}")
endif()

foreach( _QT_DIR ${_QT_DIRS} )
  if( IS_DIRECTORY ${_QT_DIR} )
    file( GLOB _QT_ARCH_DIRS LIST_DIRECTORIES true "${_QT_DIR}/*" )
    foreach( _QT_ARCH_DIR ${_QT_ARCH_DIRS} )
      find_path( _QT_BIN_DIR "bin" PATHS "${_QT_ARCH_DIR}" NO_DEFAULT_PATH )
      if( _QT_BIN_DIR )
        if ("${CMAKE_GENERATOR_PLATFORM}" STREQUAL "Win32")
          if (NOT ${_QT_BIN_DIR} MATCHES "${QT_COMPILER}_64$")
            list ( APPEND CMAKE_PREFIX_PATH ${_QT_BIN_DIR} )
          endif ()
        elseif (${_QT_BIN_DIR} MATCHES "${QT_COMPILER}_64$")
          list ( APPEND CMAKE_PREFIX_PATH ${_QT_BIN_DIR} )
        endif ()
      endif()
      unset( _QT_BIN_DIR CACHE )
    endforeach()
  endif()
endforeach()

# highest Qt versions should be first
list(SORT CMAKE_PREFIX_PATH COMPARE NATURAL ORDER DESCENDING)

# use default script to select version
if ( "${Qt5_FIND_COMPONENTS}" )
  find_package(Qt5)
else()
  set( Qt5_FIND_COMPONENTS Charts Concurrent Core Gui Network OpenGL PrintSupport Qml QmlModels QmlWorkerScript Quick QuickControls2 QuickTemplates2 QuickWidgets SerialBus SerialPort Sql Svg VirtualKeyboard Widgets Xml XmlPatterns )
  find_package(Qt5 COMPONENTS ${Qt5_FIND_COMPONENTS} REQUIRED )
endif()
find_package(Qt5LinguistTools)
message( STATUS "Qt5 version: ${Qt5_VERSION}" )

list( APPEND Qt5_COMPONENTS ${Qt5_FIND_COMPONENTS} )
list( REMOVE_DUPLICATES Qt5_COMPONENTS )

# reset prefix path
set( CMAKE_PREFIX_PATH ${_BAK_CMAKE_PREFIX_PATH} )

# reset custom module path
set( CMAKE_MODULE_PATH ${_BAK_CMAKE_MODULE_PATH} )
