#################################################
# cmake file for building procctrl package
# @author Eté Rémi
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( procctrl )

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
  MESSAGE( SEND_ERROR "procctrl requires C++11 support. Please upgrade your compiler !" )
ENDIF()

# --------------- cmake settings -----------
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# load default settings
INCLUDE( default_settings )

# --------------- options -----------
OPTION( BUILD_WEB_INTERFACE "Whether to build Wt web client interface" OFF )
MESSAGE( STATUS "BUILD_WEB_INTERFACE : ${BUILD_WEB_INTERFACE}" )

OPTION( PROCCTRL_TESTS "Whether to build and run unit tests" OFF )
MESSAGE( STATUS "PROCCTRL_TESTS : ${PROCCTRL_TESTS}" )

###############################
# Find Threads or multithreading.
FIND_PACKAGE( Threads REQUIRED )

###############################
# Find DIM package
FIND_PACKAGE( DIM REQUIRED SHARED )
FIND_PACKAGE( JSONCPP REQUIRED )
FIND_PACKAGE( MySQL REQUIRED )
FIND_PACKAGE( OpenSSL REQUIRED )

INCLUDE_DIRECTORIES( ${DIM_INCLUDE_DIRS} )
LINK_LIBRARIES( ${DIM_LIBRARIES} )
ADD_DEFINITIONS ( ${DIM_DEFINITIONS} )

INCLUDE_DIRECTORIES( ${JSONCPP_INCLUDE_DIRS} )
LINK_LIBRARIES( ${JSONCPP_LIBRARIES} )
ADD_DEFINITIONS ( ${JSONCPP_DEFINITIONS} )

INCLUDE_DIRECTORIES( ${MySQL_INCLUDE_DIRS} )
LINK_LIBRARIES( ${MySQL_LIBRARIES} )
ADD_DEFINITIONS ( ${MySQL_DEFINITIONS} )

INCLUDE_DIRECTORIES( ${OPENSSL_INCLUDE_DIR} )
LINK_LIBRARIES( ${OPENSSL_LIBRARIES} )

IF( CMAKE_USE_PTHREADS_INIT )
    ADD_DEFINITIONS( -lpthread )
ENDIF()

###############################
# Export ${PROJECT_NAME}_DEPENDS* to ${PROJECT_NAME}Config.cmake
SET( ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS  ${DIM_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS} ${MySQL_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
SET( ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS  ${DIM_LIBRARY_DIRS} ${JSONCPP_LIBRARY_DIRS} )
SET( ${PROJECT_NAME}_DEPENDS_LIBRARIES     ${DIM_LIBRARIES}    ${JSONCPP_LIBRARIES}    ${MySQL_LIBRARIES} ${OPENSSL_LIBRARIES})

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

ADD_SHARED_LIBRARY( procctrl ${SRC_FILES} )

INSTALL(
    TARGETS procctrl
    LIBRARY DESTINATION lib
)

MACRO( BUILD_EXECUABLE _path_ _name_ _dir_ )
	ADD_EXECUTABLE( ${_name_} ${_path_}/${_name_}.cc )
	TARGET_LINK_LIBRARIES( ${_name_} procctrl )

	INSTALL(
	    TARGETS ${_name_}
	    RUNTIME DESTINATION ${_dir_}
	)
ENDMACRO()

###############################
BUILD_EXECUABLE( source/main procctrl-srv bin )
BUILD_EXECUABLE( source/main procctrl-test-login bin )
BUILD_EXECUABLE( source/main procctrl-create-group bin )
BUILD_EXECUABLE( source/main procctrl-remove-group bin )
BUILD_EXECUABLE( source/main procctrl-list-groups bin )
BUILD_EXECUABLE( source/main procctrl-check-group-pwd bin )
BUILD_EXECUABLE( source/main procctrl-subscribe-process-status bin )

# shut it down for the moment ...
SET( BUILD_WEB_INTERFACE OFF )

# force web interface build to OFF for the moment ...
IF( BUILD_WEB_INTERFACE )
	FIND_PACKAGE( Wt REQUIRED )
	INCLUDE_DIRECTORIES( ${Wt_INCLUDE_DIRS} )
	LINK_LIBRARIES( ${Wt_LIBRARIES} )
	ADD_DEFINITIONS ( ${Wt_DEFINITIONS} )

  INCLUDE_DIRECTORIES( source/wt/include BEFORE )

  FILE( GLOB WT_SRC_FILES source/wt/src/*.cc )

  ADD_EXECUTABLE( procctrl-web-page source/wt/main/procctrl-web-page.cc ${WT_SRC_FILES} )
  TARGET_LINK_LIBRARIES( procctrl-web-page procctrl )

  INSTALL(
      TARGETS procctrl-web-page
      RUNTIME DESTINATION bin
  )

  TARGET_LINK_LIBRARIES( ${_name_} procctrl )

ENDIF()

# FILE(
# 	GLOB
# 	SCRIPTS_FILES
# 	scripts/*
# )
# INSTALL( PROGRAMS ${SCRIPTS_FILES} PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE DESTINATION bin )

###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
