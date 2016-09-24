#include <cmath>
#include <model_renderer.hpp>

void ModelRenderer::init(unsigned int _width, unsigned int _height,
			 const string &modelName,
			 float hFOV, float vFOV, float _near, float _far)
{
  width=_width;
  height=_height;
  near = _near;
  far = _far;
  glModel.initScene(width, height, hFOV, vFOV, near, far);
  loader.load(modelName, DEFAULT_SKELETON_NAME, DEFAULT_MESH_NAME, glModel, skeleton);
}


int ModelRenderer::getWidth(){return width;}
int ModelRenderer::getHeight(){return height;}
float ModelRenderer::getNear(){return near;}
float ModelRenderer::getFar(){return far;}


void ModelRenderer::setPose(Matrix4f *transforms)
{
  skeleton.anim(transforms);
}


void ModelRenderer::render(unsigned char *color, unsigned short *depth)
{
  float scale = 1.0f/(float)((2<<15)-1);
  unsigned short bg=(2<<15)-1;

  // Apply mesh2bone transform
  for (int i=0; i<skeleton.getBonesNum(); i++)
  {
    ModelBone &bone = skeleton.getBone(i);
    bone.base *= bone.meshToBoneMatrix;
  }
  glModel.updateBones(skeleton.getRawBones());
  glModel.render(color, depth);

  // zbuffer depth values are in the range [0,2^16-1], with near depth mapped to 0
  // and far depth mapped to 2^16-1. Scale back the values in the range [near, far]
  // and convert the depth in mm.
  // Note: loop parallelized through OpenMP
  // TODO: find a way to scale the values directly in OpenGL
  int totPixels = width*height;

  //#pragma omp parallel for
  for (int i=0; i<totPixels; i++)
  {
    if (depth[i]==bg)
    {
      depth[i]=0;
    }
    else
    {
      float tmp = float(depth[i]);
      // Normalize
      tmp *= scale;
      // Window coordinates -> NDC coordinates
      tmp = 2*tmp-1.0f;
      // NDC coordinates -> eye coordinates
      tmp = (tmp+(far+near)/(far-near))*(far-near)/(-2.0f)*10.0f;
      depth[i] = (unsigned short)roundf(-tmp);
    }
  }
}


unsigned int ModelRenderer::getBonesNum()
{
  return skeleton.getBonesNum();
}

const string& ModelRenderer::getBoneName(unsigned int idx)
{
  ModelBone &bone = skeleton.getBone(idx);
  return bone.name;
}

int ModelRenderer::getBoneIdx(const string &name)
{
  for (int i=0; i<skeleton.getBonesNum(); i++)
  {
    ModelBone &bone = skeleton.getBone(i);
    if (bone.name == name)
    {
      return bone.idx;
    }
  }
  return -1;
}

const Vector4f& ModelRenderer::getBoneHead(unsigned int idx)
{
  return skeleton.getBone(idx).head;
}

const Vector4f& ModelRenderer::getBoneTail(unsigned int idx)
{
  return skeleton.getBone(idx).tail;
}

const Map<Matrix4f>& ModelRenderer::getBoneBase(unsigned int idx)
{
  return skeleton.getBone(idx).base;
}

void ModelRenderer::setBonesLen(const float *bonesLen)
{
  for (int i=0; i<skeleton.getBonesNum(); i++)
  {
    ModelBone &bone = skeleton.getBone(i);
    float scaleFactor = (bone.length) ? bonesLen[i]/bone.length : 1.0;
    Matrix4f scaleMat = Matrix4f::Identity()*scaleFactor;
    scaleMat(3, 3) = 1.0f;
    //Matrix4f scaleMat = Matrix4f::Identity();
    //scaleMat(1, 1) = scaleFactor;
    bone.length = bonesLen[i];
    bone.meshToBoneMatrix = scaleMat*bone.meshToBoneMatrix;
    for (int i=0; i<bone.nChildren; i++)
    {
      ModelBone *children = bone.children[i];
      children->parentRelativeMatrix(0,3)*=scaleFactor;
      children->parentRelativeMatrix(1,3)*=scaleFactor;
      children->parentRelativeMatrix(2,3)*=scaleFactor;
    }
  }
}

void ModelRenderer::clean()
{
  glModel.clean();
  skeleton.clean();
}
