# message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")
find_package(OpenCV REQUIRED) # PATHS "${OpenCV_DIR}")
message(STATUS "OpenCV_INSTALL_PATH: ${OpenCV_INSTALL_PATH}")
message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
prepend_include_directories_if_necessary("${OpenCV_INCLUDE_DIRS}")