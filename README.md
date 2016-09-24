# GroundTruthGenerator: a set of tools to generate synthetic RGBD datasets for Computer Vision applications

The GroundTruthGenerator project provides a small set of routines for generating synthetic datasets of RGB-D images. The main component of the library is the ModelRenderer C++ class where the rendering and animation are implemented. C and Matlab interfaces are provided as well.

## RGBD dataset
An RGBD dataset consists of a (potentially large) set of image pairs, where each pair contains:
- a depthmap, typically a single-channel 16 bit image, where each pixel stores the distance of the object from the camera;
- a labels image, where the pixel colour represents the class of the object portion the pixel belongs to (e.g. different anatomic parts of the human body) or the class of the object as a whole (e.g. different objects within a room).

## Features
What the library does:
- Fast rendering and animation using OpenGL
- Supporting arbitrary 3D model formats through the [Assimp library ](http://www.assimp.org/)

What the library does not do:
- Performing realistic rendering. Only simple flat shading rendering of a single object within the scene is supported
- Loading any sort of 3D models. Some constraints on the model's mesh and skeleton are defined (see below).
 
## Dependencies
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
- [Assimp](http://www.assimp.org/)
- [GLEW](http://glew.sourceforge.net/)
- [GLFW](http://www.glfw.org/)
- [OpenCV](http://opencv.org/) version 2.4.x, modules highgui imgproc
- [CMake](https://cmake.org/) for compilation
- Matlab (for matlab wrappers) 

## Supported platforms
The library has been tested on both Linux (GCC) and Windows (Visual Studio 2013).

## Documentation
Read the inline comments in the test.m matlab script to learn about the Matlab API. The Doxygen documentation of the C wrapper is provided as well.

## Restrictions
The ModelRenderer library is able to load any 3D model file containing a scene with a single object, as long as the following restrictions are satisfied:
- the file contains a mesh named "ModelMesh"
- the file contains a skeleton named "Model"
- the skeleton root node contributes in the animation of the mesh
- the skeleton has less than 24 bones.

Finally, the OpenGL shader files stored in the shaders subdirectory (default_frag_shader.fs and default_vert_shader.vs) must be copied into the current work directory.

Two example models (a human hand and a human whole body), obtained by modifying the [MakeHuman](http://www.makehuman.org/) default model, are included in the test_models directory.

## Citing
Please cite the following work if you use this library in any scientific publication

Daniele Pianu, Roberto Nerino, Claudia Ferraris, and Antonio Chimienti 
**A novel approach to train random forests on GPU for computer vision applications using local features**
*International Journal of High Performance Computing Applications*
December 29, 2015 doi:10.1177/1094342015622672
[abstract](http://hpc.sagepub.com/content/early/2015/12/29/1094342015622672.abstract) [bib](http://hpc.sagepub.com/citmgr?type=bibtex&gca=sphpc%3B1094342015622672v1)
