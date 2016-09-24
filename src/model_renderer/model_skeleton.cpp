#include <cstring>
#include <model_skeleton.hpp>

void ModelSkeleton::init(unsigned int _nBones)
{
  nBones = _nBones;
  bones = new ModelBone[nBones];
  rawBonesMatrix = new float[nBones*16];
}


void ModelSkeleton::clean()
{
  delete []bones;
  delete []rawBonesMatrix;
}


void ModelSkeleton::addBone(ModelBone &bone, unsigned int idx)
{
  assert(idx>=0 && idx<nBones);
  bones[idx].idx = idx;
  bones[idx].name = bone.name;
  bones[idx].length = bone.length;
  bones[idx].parentRelativeMatrix = bone.parentRelativeMatrix;
  bones[idx].meshToBoneMatrix = bone.meshToBoneMatrix;
  new (&bones[idx].base) Map<Matrix4f>(rawBonesMatrix+16*idx);
  
  // Empty parent and children related members (they must be explicitely set by
  // the addChildren method)
  bones[idx].parent = NULL;
  bones[idx].nChildren = 0;
  memset(bones[idx].children, 0, sizeof(ModelBone*)*MAX_CHILDREN);
}


void ModelSkeleton::addChildren(unsigned int boneIdx, unsigned int childrenIdx)
{
  assert(boneIdx>=0 && boneIdx<nBones);
  assert(childrenIdx>=0 && childrenIdx<nBones);

  bones[childrenIdx].parent = &bones[boneIdx];
  bones[boneIdx].children[bones[boneIdx].nChildren] = &bones[childrenIdx];
  bones[boneIdx].nChildren++;
}


void _recursiveAnimSkeleton(ModelBone *bone, Matrix4f *bonesTransform);
void ModelSkeleton::anim(Matrix4f *bonesTransform)
{
  ModelBone *root = bones;
  root->base.setIdentity();
  _recursiveAnimSkeleton(root, bonesTransform);
}


void _recursiveAnimSkeleton(ModelBone *bone, Matrix4f *bonesTransform)
{
  unsigned int idx = bone->idx;

  bone->base *= (idx) ? 
    bone->parentRelativeMatrix * bonesTransform[idx] :
    bonesTransform[idx] * bone->parentRelativeMatrix;

  // Update head and tail position
  Vector4f tmp;
  tmp << 0.0f, 0.0f, 0.0f, 1.0f;
  bone->head = bone->base * tmp;
  tmp.y() = bone->length;
  bone->tail = bone->base * tmp;

  for (int i=0; i<bone->nChildren; i++)
  {
    unsigned int childIdx = bone->children[i]->idx;
    bone->children[i]->base = bone->base;
    _recursiveAnimSkeleton(bone->children[i], bonesTransform);
  }
}
