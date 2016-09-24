cmake_minimum_required (VERSION 3.3.2)

project (ModelRenderer)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

find_package(Eigen REQUIRED)
find_package(Assimp REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLFW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCVHighGuiImgProc REQUIRED)

# Set output directories
# - binaries (e.g. tests) in /bin
# - libraries (static and shared) and debug info in /lib
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_BINARY_DIR}/lib")
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

if (WIN32)
  # Enable cmath macros in Visual Studio
  add_definitions(-D_USE_MATH_DEFINES)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /openmp")
else (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif (WIN32)

set(MODEL_RENDERER_SRC_DIR "${CMAKE_SOURCE_DIR}/src/model_renderer")

include_directories(${OpenGL_INCLUDE_DIRS} ${ASSIMP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${OPENCV_INCLUDE_DIR} ${MODEL_RENDERER_SRC_DIR} ${MODEL_RENDERER_SRC_DIR}/../)
link_directories(${ASSIMP_LIBRARY_DIRS} ${GLEW_LIBRARY_DIRS} ${GLFW_LIBRARY_DIRS} ${OPENCV_LIBRARY_DIRS}
                 ${CMAKE_BINARY_DIR}/lib)

if (WIN32)
  add_library(ModelRenderer 
    ${MODEL_RENDERER_SRC_DIR}/model_skeleton.cpp
    ${MODEL_RENDERER_SRC_DIR}/gl_model.cpp
    ${MODEL_RENDERER_SRC_DIR}/assimp_model_loader.cpp
    ${MODEL_RENDERER_SRC_DIR}/model_renderer.cpp
    ${MODEL_RENDERER_SRC_DIR}/model_renderer_cwrapper.cpp)
  set(MODEL_RENDERER_LIBRARIES ModelRenderer${CMAKE_STATIC_LIBRARY_SUFFIX})
else (WIN32)
  add_library(ModelRenderer SHARED 
    ${MODEL_RENDERER_SRC_DIR}/model_skeleton.cpp
    ${MODEL_RENDERER_SRC_DIR}/gl_model.cpp
    ${MODEL_RENDERER_SRC_DIR}/assimp_model_loader.cpp
    ${MODEL_RENDERER_SRC_DIR}/model_renderer.cpp
    ${MODEL_RENDERER_SRC_DIR}/model_renderer_cwrapper.cpp)
  set(MODEL_RENDERER_LIBRARIES ${CMAKE_SHARED_LIBRARY_PREFIX}ModelRenderer${CMAKE_SHARED_LIBRARY_SUFFIX})
endif (WIN32)
target_link_libraries(ModelRenderer ${ASSIMP_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${OPENCV_LIBRARIES})

if (MATLAB_WRAPPERS)
   find_package(Matlab COMPONENTS MX_LIBRARY REQUIRED)
   set(MODEL_RENDERER_MEX_FUNCS new
			        delete
			        setpose
			        render
			        getbonesnum
			        getbonename
			        getboneidx)
   foreach (MEX_FUNC ${MODEL_RENDERER_MEX_FUNCS})
       matlab_add_mex(NAME model_renderer_${MEX_FUNC}
                      SRC "${CMAKE_SOURCE_DIR}/src/wrappers/matlab/model_renderer_${MEX_FUNC}.c"
  		      LINK_TO ${ASSIMP_LIBRARIES} ${GLEW_LIBRARIES} ${GLFW_LIBRARIES} ${OPENGL_LIBRARIES} ${OPENCV_LIBRARIES} ${MODEL_RENDERER_LIBRARIES} ${Matlab_MX_LIBRARY})
       add_dependencies(model_renderer_${MEX_FUNC} ModelRenderer)
   endforeach (MEX_FUNC MODEL_RENDERER_MEX_FUNCS)
   
   add_custom_target(matlab_wrappers
		     DEPENDS model_renderer_new
		             model_renderer_delete
			     model_renderer_setpose
		             model_renderer_render
		             model_renderer_getbonesnum
		             model_renderer_getbonename
		             model_renderer_getboneidx)
   
endif (MATLAB_WRAPPERS)
