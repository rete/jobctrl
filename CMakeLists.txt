#################################################
# cmake file for building jobctrl package
# @author Eté Rémi
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( jobctrl )

SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

###############################
# Require C++11
INCLUDE( CheckCXXCompilerFlag )

CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)

IF( COMPILER_SUPPORTS_CXX11 )
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE()
  MESSAGE( SEND_ERROR "DQMCore requires C++11 support. Please upgrade your compiler !" )
ENDIF()

# --------------- cmake settings -----------
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# load default settings
INCLUDE( default_settings )

###############################
# Find Threads or multithreading.
FIND_PACKAGE( Threads REQUIRED )

###############################
# Find DIM package
FIND_PACKAGE( DIM REQUIRED SHARED )
FIND_PACKAGE( JSONCPP REQUIRED )
FIND_PACKAGE( MySQL REQUIRED )

INCLUDE_DIRECTORIES( ${DIM_INCLUDE_DIRS} )
LINK_LIBRARIES( ${DIM_LIBRARIES} )
ADD_DEFINITIONS ( ${DIM_DEFINITIONS} ) 

INCLUDE_DIRECTORIES( ${JSONCPP_INCLUDE_DIRS} )
LINK_LIBRARIES( ${JSONCPP_LIBRARIES} )
ADD_DEFINITIONS ( ${JSONCPP_DEFINITIONS} ) 

INCLUDE_DIRECTORIES( ${MySQL_INCLUDE_DIRS} )
LINK_LIBRARIES( ${MySQL_LIBRARIES} )
ADD_DEFINITIONS ( ${MySQL_DEFINITIONS} ) 

IF( CMAKE_USE_PTHREADS_INIT )
    ADD_DEFINITIONS( -lpthread )
ENDIF()

###############################
# Export ${PROJECT_NAME}_DEPENDS* to ${PROJECT_NAME}Config.cmake
SET( ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS  ${DIM_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} )
SET( ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS  ${DIM_LIBRARY_DIRS} ${JSONCPP_LIBRARY_DIRS} )
SET( ${PROJECT_NAME}_DEPENDS_LIBRARIES     ${DIM_LIBRARIES}    ${JSONCPP_LIBRARIES}    )

###############################
# include directories
INCLUDE_DIRECTORIES( source/include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type")

FILE(
	GLOB_RECURSE
	SRC_FILES
	source/src/*.cc
)

ADD_SHARED_LIBRARY( jobctrl ${SRC_FILES} )

INSTALL( 
    TARGETS jobctrl
    LIBRARY DESTINATION lib
)

#ADD_EXECUTABLE( dimjc_daemon src/djc.cxx )
#TARGET_LINK_LIBRARIES( dimjc_daemon dimjc )

#INSTALL( 
#    TARGETS dimjc_daemon
#    RUNTIME DESTINATION bin
#)

###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

