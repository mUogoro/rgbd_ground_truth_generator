# Try to find the GLEW library
#
# Once done this will define
# GLEW_FOUND - System has the GLEW library installed
# GLEW_INCLUDE_DIRS - The GLEW library include directories
# GLEW_LIBRARY_DIRS - The GLEW library library directories
# GLEW_LIBRARIES - The libraries needed to use GLEW library

if (WIN32)
  set(GLEW_PATH_GUESS "C:/glew" "C:/Program Files/glew" "C:/Program Files (x86)/glew")
  find_path(GLEW_INCLUDE_DIR glew.h GLEW.h
            PATHS ${GLEW_PATH_GUESS}
	    PATH_SUFFIXES "include" "include/GLEW" "include/GL")
  find_library(GLEW_LIBRARY NAMES glew glew32
               PATHS ${GLEW_PATH_GUESS}
               PATH_SUFFIXES "lib" "lib/Release" "lib/Release/x64")
elseif (UNIX AND NOT APPLE)
  find_path(GLEW_INCLUDE_DIR glew.h
            PATHS "/usr/include"
	    PATH_SUFFIXES "GL" "GLEW")
  find_library(GLEW_LIBRARY NAMES glew
               PATHS "/usr/lib64" "/usr/lib")
endif (WIN32)
			   
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW DEFAULT_MSG
                                  GLEW_LIBRARY GLEW_INCLUDE_DIR)
mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY)

get_filename_component(GLEW_LIBRARIES ${GLEW_LIBRARY} NAME)
get_filename_component(GLEW_LIBRARY_DIRS ${GLEW_LIBRARY} DIRECTORY)
set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})