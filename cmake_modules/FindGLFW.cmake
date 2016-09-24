# Try to find the GLFW library
#
# Once done this will define
# GLFW_FOUND - System has the GLFW library installed
# GLFW_INCLUDE_DIRS - The GLFW library include directories
# GLFW_LIBRARY_DIRS - The GLFW library library directories
# GLFW_LIBRARIES - The libraries needed to use GLFW library

if (WIN32)
  set(GLFW_PATH_GUESS "C:/glfw" "C:/Program Files/glfw" "C:/Program Files (x86)/glfw")
  find_path(GLFW_INCLUDE_DIR glfw3.h
            PATHS ${GLFW_PATH_GUESS}
	    PATH_SUFFIXES "include" "include/GLFW")
  # Check for specific VS library (so far only VS 2012 2013 2015 are checked)
  if (MSVC11)
    set(LIB_SUFFIX "-vc2012")
  elseif (MSVC12)
    set(LIB_SUFFIX "-vc2013")
  elseif (MSVC14)
    set(LIB_SUFFIX "-vc2015")
  endif()
  find_library(GLFW_LIBRARY NAMES glfw3
               PATHS ${GLFW_PATH_GUESS}
               PATH_SUFFIXES "lib${LIB_SUFFIX}")
elseif (UNIX AND NOT APPLE)
  find_path(GLFW_INCLUDE_DIR glfw3.h
            PATHS "/usr/include"
	    PATH_SUFFIXES "GL" "GLFW")
  find_library(GLFW_LIBRARY NAMES glfw3
               PATHS "/usr/lib64" "/usr/lib")
endif (WIN32)
			   
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLFW DEFAULT_MSG
                                  GLFW_LIBRARY GLFW_INCLUDE_DIR)
mark_as_advanced(GLFW_INCLUDE_DIR GLFW_LIBRARY)

get_filename_component(GLFW_LIBRARIES ${GLFW_LIBRARY} NAME)
get_filename_component(GLFW_LIBRARY_DIRS ${GLFW_LIBRARY} DIRECTORY)
set(GLFW_INCLUDE_DIRS ${GLFW_INCLUDE_DIR})