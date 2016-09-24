#ifndef __CG_MODEL_HPP
#define __CG_MODEL_HPP

class CGModel
{
public:
  virtual void initScene(unsigned int width, unsigned int height,
			 float hFOV, float vFOV, float near, float far)=0;
  virtual void initModel(unsigned int nVertices,
			 unsigned int nFaces,
			 unsigned int nBones,
			 unsigned int maxBonesPerVert,
			 unsigned int nMaterials)=0;
  virtual void clean()=0;
  virtual void fillVertices(const float *vertices)=0;
  virtual void fillVertexAttributes(const unsigned int *bonesIdx,
				    const float *weights,
				    const unsigned int *materialsIdx)=0;
  virtual void fillFaces(const unsigned int *faces)=0;
  virtual void fillMaterials(const float *materials)=0;
  virtual void updateBones(const float *bones)=0;
  virtual void render(void *buffer=NULL, void *zbuffer=NULL)=0;
};

#endif // __CG_MODEL_HPP
