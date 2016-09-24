#ifndef __MODEL_SKELETON_HPP
#define __MODEL_SKELETON_HPP

#include <string>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#define MAX_CHILDREN (16)

/*! \brief Bone class
 * Class representing a single bone in the skeleton hierarchy
 *
 */
typedef struct ModelBone
{
  string name;                      /*!< Bone name */
  unsigned int idx;                 /*!< Bone unique index */
  float length;                     /*!< Bone length */
  Vector4f head;                    /*!< Bone's joint of rotation */
  Vector4f tail;                    /*!< Bone end-point, oriented towards local y */
  Matrix4f parentRelativeMatrix;    /*!< Transformation matrix relative the parent bone */
  Matrix4f meshToBoneMatrix;        /*!< Transformation matrix from mesh to bone coordinate system*/
  Map<Matrix4f> base;               /*!< Bone's system of reference (world space) */
  ModelBone *parent;                 /*!< Pointer to parent bone */
  unsigned int nChildren;           /*!< Number of children bones */
  ModelBone *children[MAX_CHILDREN]; /*!< Vector of pointers to children bones */
  ModelBone() : base(Map<Matrix4f>(NULL)){}
} ModelBone;


/*! \brief Model skeleton class
 * The class represent a model skeleton as a hierarchy of bones
 *
 */
class ModelSkeleton
{
private:
  ModelBone *bones;
  float *rawBonesMatrix;
  unsigned int nBones;
public:
  void init(unsigned int nBones);
  void clean();
  void addBone(ModelBone &bone, unsigned int idx);
  void addChildren(unsigned int boneIdx, unsigned int childrenIdx);
  void anim(Matrix4f *bonesTransform);

  ModelBone& getBone(unsigned int idx) const {return bones[idx];}
  unsigned int getBonesNum() const {return nBones;}
  const float * getRawBones() const {return rawBonesMatrix;};
};


#endif // __MODEL_SKELETON_HPP
