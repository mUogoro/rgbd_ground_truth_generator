#ifndef __MODEL_RENDERER_HPP
#define __MODEL_RENDERER_HPP

#include <Eigen/Dense>
#include <gl_model.hpp>
#include <model_skeleton.hpp>
#include <assimp_model_loader.hpp>


#define DEFAULT_MODEL_FILE ("model.dae")
#define DEFAULT_SKELETON_NAME ("Model")
#define DEFAULT_MESH_NAME ("ModelMesh")

#define DEFAULT_WIDTH (640)
#define DEFAULT_HEIGHT (480)

#define DEFAULT_HFOV (57.0f)
#define DEFAULT_VFOV (43.0f)

#define DEFAULT_NEAR (30.0f)
#define DEFAULT_FAR (80.0f)

using namespace Eigen;

class ModelRenderer{
private:
  GLModel glModel;
  ModelSkeleton skeleton;
  AssimpModelLoader loader;
  float width, height, near, far;
public:
  void init(unsigned int width=DEFAULT_WIDTH, unsigned int height=DEFAULT_HEIGHT,
	    const string &modelName=DEFAULT_MODEL_FILE,
	    float hFOV=DEFAULT_HFOV, float vFOV=DEFAULT_VFOV,
	    float near=DEFAULT_NEAR, float far=DEFAULT_FAR);
  int getWidth();
  int getHeight();
  float getNear();
  float getFar();
  void setPose(Matrix4f *transforms);
  void render(unsigned char *color, unsigned short *depth);
  unsigned int getBonesNum();
  const string& getBoneName(unsigned int idx);
  int getBoneIdx(const string &name);
  const Vector4f& getBoneHead(unsigned int idx);
  const Vector4f& getBoneTail(unsigned int idx);
  const Map<Matrix4f>& getBoneBase(unsigned int idx);
  void setBonesLen(const float *bonesLen);
  void clean();
};

#endif // __MODEL_RENDERERHPP
