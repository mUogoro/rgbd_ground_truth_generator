#ifndef __GL_MODEL_HPP
#define __GL_MODEL_HPP

#include <glew.h>
#include <glfw3.h>
#include <cg_model.hpp>

class GLModel: public CGModel
{
private:
  GLFWwindow *glWindow;
  unsigned int width, height;

  GLuint vboId;
  GLuint fboId, rboColorId, rboDepthId;
  GLuint verticesVbo;
  GLuint modelVsId, modelFsId, modelProgId;
  GLuint vsId, fsId, progId;
  GLuint vertBuff, facesBuff, vertBonesIdBuff, vertBonesWeightBuff, vertMaterialIdBuff; 

  GLfloat projectionMat[16];
  GLuint positionAttrib, vertBonesId1Attrib, vertBonesId2Attrib,
    vertBonesWeight1Attrib, vertBonesWeight2Attrib, vertMaterialIdAttrib;
  GLuint projectionMatUnif, bonesMatUnif, materialsUnif;

  unsigned int nVertices, nFaces, nBones, maxBonesPerVert, nMaterials;

public:
  void initScene(unsigned int width, unsigned int height,
		 float hFOV, float vFOV,
		 float near, float far);
  void initModel(unsigned int nVertices,
		 unsigned int nFaces,
		 unsigned int nBones,
		 unsigned int maxBonesPerVert,
		 unsigned int nMaterials);
  void clean();
  void fillVertices(const float *vertices);
  void fillFaces(const unsigned int *faces);
  void fillVertexAttributes(const unsigned int *bonesIdx,
			    const float *weights,
			    const unsigned int *materialsIdx);
  void fillMaterials(const float *materials);
  void updateBones(const float *bones);
  void render(void *buffer=NULL, void *zbuffer=NULL);
};

#endif
