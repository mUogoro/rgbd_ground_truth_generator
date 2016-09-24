#ifndef __ASSIMP_MODEL_LOADER
#define __ASSIMP_MODEL_LOADER

#include <model_loader.hpp>

class AssimpModelLoader: public ModelLoader
{
public:
  bool load(const string &handFile, const string &skeletonName, const string &meshName,
	        CGModel &cghand, ModelSkeleton &skeleton);
};

#endif // __ASSIMP_MODEL_LOADER
