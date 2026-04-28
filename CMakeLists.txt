# Defines AppBase library target.
#
# Dependencies (must be provided by parent project when used as submodule):
#   - hive_utilities: Required for logging config, notifications, data collection
#   - fc: Transitively required via hive_utilities (logging, serialization)
#
# When used as a submodule, ensure these targets exist before add_subdirectory(appbase).
#
project( AppBase )
cmake_minimum_required( VERSION 2.8.12 )

file(GLOB HEADERS "include/appbase/*.hpp")

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

if( APPLE )
  # Apple Specific Options Here
  message( STATUS "Configuring AppBase on OS X" )
  # -Wno-missing-template-arg-list-after-template-kw silences a Clang 17+
  # diagnostic triggered by Boost 1.88's segment_manager.hpp.
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -Wall -Wno-conversion -Wno-deprecated-declarations -Wno-missing-template-arg-list-after-template-kw" )
else( APPLE )
  # Linux Specific Options Here
  message( STATUS "Configuring AppBase on Linux" )
  set( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall" )
  set( rt_library rt )
  set( pthread_library pthread)
  if ( FULL_STATIC_BUILD )
    set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
  endif ( FULL_STATIC_BUILD )
endif( APPLE )


if(ENABLE_COVERAGE_TESTING)
    SET(CMAKE_CXX_FLAGS "--coverage ${CMAKE_CXX_FLAGS}")
endif()

add_library( appbase STATIC
             signals_handler.cpp
             application.cpp
             ${HEADERS}
           )

target_link_libraries( appbase PUBLIC hive_utilities )

target_include_directories( appbase
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

INSTALL( TARGETS
   appbase

   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib
)
INSTALL( FILES ${HEADERS} DESTINATION "include/appbase" )

add_subdirectory( examples )
