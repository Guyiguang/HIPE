include(GetPrerequisites)
include("${PROJECT_SOURCE_DIR}/../../cmake/list_dependencies.cmake")
#getDependencies
macro(hipe_find_dependencies list_deps target_name BUILD_CONFIG)
  if (WIN32)
	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG}/${target_name}${EXT_BIN}" cm_path)
	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/bin/${BUILD_CONFIG}" cm_dir)
	list(APPEND PATH_SHAREDLIB "${Hipecore_DIR}/bin/${BUILD_CONFIG}")
  else()
	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/lib/${BUILD_CONFIG}/lib${target_name}${EXT_BIN}" cm_path)
	file(TO_CMAKE_PATH "${CMAKE_INSTALL_PREFIX}/lib/${BUILD_CONFIG}" cm_dir)
	list(APPEND PATH_SHAREDLIB "${Hipecore_DIR}/lib/${BUILD_CONFIG}")

  endif()
	
	list(APPEND PATH_SHAREDLIB "${cm_dir}")

	message(STATUS "SEARCH PATH : ${PATH_SHAREDLIB}")

	message(STATUS "Info file path [ ${cm_path} ] in dir [ ${cm_dir} ] ")
	get_prerequisites("${cm_path}" PREREQS 1 1 "" "${PATH_SHAREDLIB}")
	
	message(STATUS "prerequisites ${PREREQS} for ${cm_path}")
	foreach(DEPENDENCY_FILE ${PREREQS})
	gp_resolve_item("${cm_path}" "${DEPENDENCY_FILE}" "" "${PATH_SHAREDLIB}" resolved_file)
	if (UNIX)
		get_filename_component( dep_realpath "${resolved_file}" REALPATH )
		get_filename_component( dep_name "${resolved_file}" NAME )
		LIST(APPEND list_deps "${dep_realpath}")

	endif()
	message(STATUS "resolved_file='${resolved_file}'")
	LIST(APPEND list_deps "${resolved_file}")
	endforeach()
endmacro()
