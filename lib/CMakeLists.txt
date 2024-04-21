message( "lib" )

find_package( Boost )
find_package( Eigen3 )
find_package( OpenCV 4 )
find_package( Qt5 COMPONENTS Core Network PrintSupport Widgets Svg REQUIRED )
find_package( Qwt)

# pull and push scripts
add_subtree_actions( "lib" "lib" "git@github.com:BertramBildverarbeitung/lib-bg.git" )

file( GLOB LIB_SRC
    "*.h"
    "*.cpp"
)

add_library ( bg STATIC ${LIB_SRC} )
set_target_properties( bg PROPERTIES VERSION ${PROJECT_VERSION} )
target_link_libraries( bg Boost::Boost Eigen3::Eigen OpenCV Qwt Qt5::Network Qt5::PrintSupport Qt5::Widgets Qt5::Svg Qt5::SerialPort )
set_target_properties(bg PROPERTIES INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/inspector")
set_target_properties(bg PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_SOURCE_DIR}/inspector")

# translations
file( GLOB TS_FILES
    "*.ts"
)
add_translations( bg ${TS_FILES} )
