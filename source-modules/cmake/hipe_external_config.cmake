cmake_minimum_required(VERSION 3.7.1)

# Check if HIPE_EXTERNAL has been set via the command-line via
#
# -DHIPE_EXTERNAL=<path>
#
# If not, check if the environment variable of the same name has been set and
# use that instead. If neither has been set, raise a fatal error.

# Unset via command-line.
if(NOT DEFINED HIPE_EXTERNAL)
  # Set via environment variable.
  if(DEFINED ENV{HIPE_EXTERNAL})
    set(HIPE_EXTERNAL "$ENV{HIPE_EXTERNAL}")
  # Unset via environment variable.
  else(DEFINED ENV{HIPE_EXTERNAL})
	set(HIPE_EXTERNAL "${CMAKE_SOURCE_DIR}/../../../3rdParty/HipeExternal")
    #message(FATAL_ERROR "Could not find \"HIPE_EXTERNAL\". Please set it locally or by environment variable.")
  endif(DEFINED ENV{HIPE_EXTERNAL})
endif(NOT DEFINED HIPE_EXTERNAL)

# Make sure that it is an absolute path.
get_filename_component(HIPE_EXTERNAL "${HIPE_EXTERNAL}" ABSOLUTE)

# Inform the user which value was used.
message(STATUS "HIPE_EXTERNAL : ${HIPE_EXTERNAL}")

# Make sure HIPE_EXTERNAL exists.
if(NOT EXISTS "${HIPE_EXTERNAL}")
  message(FATAL_ERROR "${HIPE_EXTERNAL} does not exist")
elseif(NOT IS_DIRECTORY "${HIPE_EXTERNAL}")
  message(FATAL_ERROR "${HIPE_EXTERNAL} is not a directory")
endif(NOT EXISTS "${HIPE_EXTERNAL}")



if(WIN32)
  set(HIPE_EXTERNAL_DIR "${HIPE_EXTERNAL}/win64")
else(WIN32)
  set(HIPE_EXTERNAL_DIR "${HIPE_EXTERNAL}/linux64")
  # Without this, the linker fails to find e.g. libboost_wave when building
  # multiple libraries ("cannot find -lboost_wave"), despite correctly locating
  # the libraries in HIPE_EXTERNAL with the find_package command. For example,
  # building streaming_rtsp or core separately succeeds but building both at the
  # same time results in the aforementioned error.
#   link_libraries("-L '${HIPE_EXTERNAL_DIR}/lib'")
endif(WIN32)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
set(CUDA_MAJOR "9" CACHE STRING "CUDA MAJOR VERSION" FORCE)
set(CUDA_MINOR "2" CACHE STRING "CUDA MINOR VERSION" FORCE)

else()
set(CUDA_MAJOR "10" CACHE STRING "CUDA MAJOR VERSION" FORCE)
set(CUDA_MINOR "0" CACHE STRING "CUDA MINOR VERSION" FORCE)

endif()

set(CUDA_VERSION "${CUDA_MAJOR}.${CUDA_MINOR}" CACHE PATH "CUDA_VERSION" FORCE)
if (WIN32)
  file(TO_CMAKE_PATH "$ENV{CUDA_PATH_V${CUDA_MAJOR}_${CUDA_MINOR}}" _CUDA_TOOLKIT_ROOT_DIR)
else()
  file(TO_CMAKE_PATH "/usr/local/cuda-${CUDA_MAJOR}.${CUDA_MINOR}" _CUDA_TOOLKIT_ROOT_DIR)
endif(WIN32)
SET(CUDA_TOOLKIT_ROOT_DIR ${_CUDA_TOOLKIT_ROOT_DIR} CACHE PATH "Cuda Tookit PATH" FORCE)
message(STATUS "Set CUDA_TOOLKIT_ROOT_DIR : ${CUDA_TOOLKIT_ROOT_DIR}")

if(WIN32)

  set(Hipecore_DIR "${CMAKE_SOURCE_DIR}/../../../install/hipe-core" CACHE PATH "hipecore" FORCE)
  list(APPEND CMAKE_PREFIX_PATH "${Hipecore_DIR}")
    
  # TODO
  # Update this section to enable optional use of installed system libraries.
  set(Boost_DIR "${HIPE_EXTERNAL_DIR}/boost_1_66_0/" CACHE PATH "Boost_DIR" FORCE)
  set(Boost_INCLUDE_DIR "${Boost_DIR}" CACHE PATH "Boost_INCLUDE_DIR" FORCE)
  set(BOOST_LIBRARYDIR "${Boost_DIR}/lib" CACHE PATH "BOOST_LIBRARYDIR" FORCE)
  set(BOOST_ROOT "${Boost_DIR}/"  CACHE PATH "BOOST_ROOT" FORCE)
  if(HIPE_EXTERNAL_BOOST)
    list(APPEND CMAKE_PREFIX_PATH "${Boost_DIR}")
  endif(HIPE_EXTERNAL_BOOST)

 
  
  set(OpenCV_DIR "${HIPE_EXTERNAL_DIR}/" CACHE PATH "OpenCV Directory" FORCE)
  if(HIPE_EXTERNAL_OPENCV)
    list(APPEND CMAKE_PREFIX_PATH "${OpenCV_DIR}")
  endif(HIPE_EXTERNAL_OPENCV)

  set(Dlib_DIR "${HIPE_EXTERNAL_DIR}/"  CACHE PATH "DLIB_LIBRARYDIR" FORCE)
  
  set(Caffe_DIR "${HIPE_EXTERNAL_DIR}/caffe"  CACHE PATH "The directory containing a CMake configuration file for Caffe." FORCE)
   
  set(GFLAGS_ROOT_DIR "${HIPE_EXTERNAL_DIR}/"  CACHE PATH "Folder contains Gflags" FORCE)
  
  set(GLOG_ROOT_DIR "${HIPE_EXTERNAL_DIR}/" CACHE PATH "Folder contains Google glog" FORCE)
  
  set(OPENBLAS_ROOT_DIR "${HIPE_EXTERNAL_DIR}/OpenBlas/" CACHE PATH "Folder contains openblas" FORCE)

  set(Python27_DIR "${HIPE_EXTERNAL_DIR}/Python2.7.14"  CACHE PATH "PYTHON_LIBRARYDIR" FORCE )
  if(HIPE_EXTERNAL_PYTHON27)
    list(APPEND CMAKE_PREFIX_PATH "${Python27_DIR}")
  endif(HIPE_EXTERNAL_PYTHON27)
  
  set(Python36_DIR "${HIPE_EXTERNAL_DIR}/Python36"  CACHE PATH "PYTHON36_LIBRARYDIR" FORCE )
  if(HIPE_EXTERNAL_PYTHON36)
    list(APPEND CMAKE_PREFIX_PATH "${Python36_DIR}")
  endif(HIPE_EXTERNAL_PYTHON36)
  
  set(OpenBLAS_DIR "${HIPE_EXTERNAL_DIR}/OpenBLAS")

else(WIN32)

  set(Hipecore_DIR "${CMAKE_SOURCE_DIR}/../../../install/hipe-core" CACHE PATH "hipecore" FORCE)
  list(APPEND CMAKE_PREFIX_PATH "${Hipecore_DIR}")

  if(HIPE_EXTERNAL_PYTHON27)
	  set(Python27_DIR "${HIPE_EXTERNAL_DIR}/python27/usr"  CACHE PATH "PYTHON27_LIBRARYDIR")
	  list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/python27/usr")
  endif(HIPE_EXTERNAL_PYTHON27)
  
  if(HIPE_EXTERNAL_PYTHON36)
	  set(Python36_DIR "${HIPE_EXTERNAL_DIR}/python36/usr"  CACHE PATH "PYTHON36_LIBRARYDIR")
	  list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/python36/usr")
  endif(HIPE_EXTERNAL_PYTHON36)
  
if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
	  set(Python36_DIR "/home/hipe-group/python-3.6.7"  CACHE PATH "PYTHON36_LIBRARYDIR")
	  list(APPEND CMAKE_PREFIX_PATH "${Python36_DIR}")
endif()

  if(HIPE_EXTERNAL_OPENCV)
    list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/cuda8")
    set(ENV{PATH} "${HIPE_EXTERNAL_DIR}/cuda8/bin:$ENV{PATH}")
    list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/opencv")
  endif(HIPE_EXTERNAL_OPENCV)

  if(HIPE_EXTERNAL_BOOST)
    list(APPEND CMAKE_PREFIX_PATH "${HIPE_EXTERNAL_DIR}/boost")
  endif(HIPE_EXTERNAL_BOOST)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(powerpc|ppc)64le")
  set(Caffe_DIR "/opt/DL/caffe-ibm/"  CACHE PATH "The directory containing a CMake configuration file for Caffe." FORCE)
else()
  set(Caffe_DIR "${HIPE_EXTERNAL_DIR}/caffe/"  CACHE PATH "The directory containing a CMake configuration file for Caffe." FORCE)
endif()

  set(GFLAGS_ROOT_DIR "${HIPE_EXTERNAL_DIR}"  CACHE PATH "Folder contains Gflags" FORCE)
  
  set(GLOG_ROOT_DIR "${HIPE_EXTERNAL_DIR}" CACHE PATH "Folder contains Google glog" FORCE)


  set(Dlib_DIR "${HIPE_EXTERNAL_DIR}/dlib" CACHE PATH "DLIB_LIBRARYDIR" FORCE)

  set(WebRTC_DIR "${HIPE_EXTERNAL_DIR}/WebRTCServer" CACHE PATH "Path to the root webrtc directory" )
  list(APPEND CMAKE_PREFIX_PATH "${WebRTC_DIR}")
  
  message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif(WIN32)
