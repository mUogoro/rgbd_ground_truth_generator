#include <Eigen/Dense>
#include <model_renderer.hpp>
#include <model_renderer.h>


using namespace Eigen;

ModelRenderer *ModelRenderer_new()
{
  ModelRenderer *model = new ModelRenderer();
  return model;
}

void ModelRenderer_initDefault(ModelRenderer *self)
{
  self->init();
}

void ModelRenderer_init(ModelRenderer *self, unsigned int width, unsigned int height,
			const char *modelName,
			float hFOV, float vFOV, float near, float far)
{
  self->init(width, height, string(modelName), hFOV, vFOV, near, far);
}

int ModelRenderer_getWidth(ModelRenderer *self)
{
  return self->getWidth();
}

int ModelRenderer_getHeight(ModelRenderer *self)
{
  return self->getHeight();
}

float ModelRenderer_getNear(ModelRenderer *self)
{
  return self->getNear();
}

float ModelRenderer_getFar(ModelRenderer *self)
{
  return self->getFar();
}

void ModelRenderer_setPose(ModelRenderer *self, float *transformations)
{
  // TODO: avoid memory copy of transformations
  Matrix4f *transformMatx = new Matrix4f[self->getBonesNum()];
  for (int i=0; i<self->getBonesNum(); i++)
  {
    transformMatx[i] = Map<Matrix4f>(transformations+i*16);
  }
  self->setPose(transformMatx);
  delete []transformMatx;
}

void ModelRenderer_render(ModelRenderer *self, unsigned char *color, unsigned short *depth)
{
  self->render(color, depth);
}

unsigned int ModelRenderer_getBonesNum(ModelRenderer *self)
{
  return self->getBonesNum();
}

void ModelRenderer_setBonesLen(ModelRenderer *self, float *bonesLen)
{
  self->setBonesLen(bonesLen);
}

int ModelRenderer_getBoneIdx(ModelRenderer *self, const char *name)
{
  return self->getBoneIdx(name);
}

const char *ModelRenderer_getBoneName(ModelRenderer *self, unsigned int idx)
{
  return self->getBoneName(idx).c_str();
}

const float *ModelRenderer_getBoneHead(ModelRenderer *self, unsigned int idx)
{
  return self->getBoneHead(idx).data();
}

const float *ModelRenderer_getBoneTail(ModelRenderer *self, unsigned int idx)
{
  return self->getBoneTail(idx).data();
}

const float *ModelRenderer_getBoneBase(ModelRenderer *self, unsigned int idx)
{
  return self->getBoneBase(idx).data();
}

void ModelRenderer_delete(ModelRenderer *self)
{
  self->clean();
  delete self;
  self = NULL;
}
