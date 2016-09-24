# Try to find the Assimp library
#
# Once done this will define
# ASSIMP_FOUND - System has the ASSIMP library installed
# ASSIMP_INCLUDE_DIRS - The ASSIMP library include directories
# ASSIMP_LIBRARY_DIRS - The ASSIMP library library directories
# ASSIMP_LIBRARIES - The libraries needed to use ASSIMP library

if (WIN32)
  set(ASSIMP_PATH_GUESS "C:/assimp" "C:/Program Files/assimp" "C:/Program Files (x86)/assimp")
  find_path(ASSIMP_INCLUDE_DIR Importer.hpp postprocess.h scene.h mesh.h
            PATHS ${ASSIMP_PATH_GUESS}
	    PATH_SUFFIXES "include" "include/assimp")
  # Check for specific VS library (so far only VS 2012 2013 2015 are checked)
  if (MSVC11)
    set(LIB_SUFFIX "-vc2012")
  elseif (MSVC12)
    set(LIB_SUFFIX "-vc2013")
  elseif (MSVC14)
    set(LIB_SUFFIX "-vc2015")
  endif()
  find_library(ASSIMP_LIBRARY NAMES assimp
               PATHS ${ASSIMP_PATH_GUESS}
               PATH_SUFFIXES "lib${LIB_SUFFIX}")
elseif (UNIX AND NOT APPLE)
  find_path(ASSIMP_INCLUDE_DIR Importer.hpp postprocess.h scene.h mesh.h
            PATHS "/usr/include" "/usr/local/include"
	    PATH_SUFFIXES "assimp")
  find_library(ASSIMP_LIBRARY NAMES assimp
               PATHS "/usr/lib64" "/usr/lib")
endif (WIN32)
			   
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ASSIMP DEFAULT_MSG
                                  ASSIMP_LIBRARY ASSIMP_INCLUDE_DIR)
mark_as_advanced(ASSIMP_INCLUDE_DIR ASSIMP_LIBRARY)

get_filename_component(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY} NAME)
get_filename_component(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY} DIRECTORY)
set(ASSIMP_INCLUDE_DIRS ${ASSIMP_INCLUDE_DIR})