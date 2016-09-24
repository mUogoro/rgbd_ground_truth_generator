#ifndef __MODEL_LOADER_HPP
#define __MODEL_LOADER_HPP

#include <string>
#include <cg_model.hpp>
#include <model_skeleton.hpp>

using namespace std;

class ModelLoader
{
public:
  virtual bool load(const string &modelFile, const string &skeletonName, const string &meshName,
		    CGModel &cgmodel, ModelSkeleton &skeleton)=0;
};

#endif // __MODEL_LOADER_HPP
