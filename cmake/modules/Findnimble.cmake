# CMake module for finding Nimble
# XXX_FOUND           		Set to false, or undefined, if we haven't found, or don't want to use XXX.
# XXX_LIBRARY				Name of XXX Library.
# XXX_INCLUDE_DIRS			The final set of include directories listed in one variable for use by client code.  This should not be a cache entry.
# XXX_LIBRARIES      		The libraries to link against to use XXX. These should include full paths.  This should not be a cache entry.
# XXX_DEFINITIONS      		Definitions to use when compiling code that uses XXX.
# XXX_LIBRARY_DIRS     		Optionally, the final set of library directories listed in one variable for use by client code.  This should not be a cache entry.
# XXX_ROOT_DIR        		Where to find the base directory of XXX.
# XXX_VERSION_YY			Expect Version YY if true. Make sure at most one of these is ever true.
# XXX_RUNTIME_LIBRARY_DIRS 	Optionally, the runtime library search path for use when running an executable linked to shared libraries.
# XXX_VERSION_STRING    	A human-readable string containing the version of the package found, if any.
# XXX_VERSION_MAJOR     	The major version of the package found, if any.
# XXX_VERSION_MINOR      	The minor version of the package found, if any.
# XXX_VERSION_PATCH       	The patch version of the package found, if any.

if(NOT nimble_FOUND)
	set(nimble_LIBRARY nimble)

	get_filename_component(thisPath ${CMAKE_CURRENT_LIST_FILE} PATH)
	set(nimble_INCLUDE_DIRS ${thisPath}/../../../../include)
	message(STATUS "Nimble headers found")
endif()