#include <map>
#include <cstring>
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp_model_loader.hpp>


using namespace std;
using namespace Assimp;


void _recursiveReadSkeletonNodes(aiNode *skeletonNode, ModelBone *bone, unsigned int *boneIdx, map<string, ModelBone*> &name2Bone);
void  _recursiveFillSkeleton(ModelSkeleton *skeleton, ModelBone *bone);
void _recursiveFreeNodes(ModelBone *bone);

bool AssimpModelLoader::load(const string &modelFile, const string &skeletonName, const string &meshName,
			    CGModel &cgmodel, ModelSkeleton &skeleton)
{
  bool retVal = false;
  Importer importer;

  const aiScene *scene = importer.ReadFile(modelFile.c_str(), aiProcess_Triangulate);
  assert(scene && scene->HasMeshes() && scene->HasMaterials());
  
  // Look for model mesh and skeleton
  aiNode *meshNode = scene->mRootNode->FindNode(meshName.c_str());
  aiNode *skeletonNode = scene->mRootNode->FindNode(skeletonName.c_str());
  assert(meshNode && skeletonNode);

  // Count the number of mesh objects stored in the mesh node. This is necessary when
  // more than one material is associated to the current mesh it get split into multiple
  // meshes.
  unsigned int nMeshes = meshNode->mNumMeshes;
  assert(nMeshes>0);

  // Iterate through the faces and:
  // - count how many vertex and faces are stored per mesh
  // - look for the model skeleton root node. The root node is considered as the
  // bone node whose parent is the skeleton node
  // - count the maximum number of bones per mesh
  unsigned int *nVertsPerMesh = new unsigned int[nMeshes];
  unsigned int *nFacesPerMesh = new unsigned int[nMeshes];
  unsigned int totVerts=0, totFaces=0, maxNBonesPerMesh=0, totMaterials=0;
  aiNode *skeletonRootNode = NULL;
  for (int i=0; i<nMeshes; i++)
  {
    aiMesh *currMesh = scene->mMeshes[meshNode->mMeshes[i]];
    if (currMesh->mNumBones>maxNBonesPerMesh)
    {
      maxNBonesPerMesh = currMesh->mNumBones;
    }
    if (!skeletonRootNode)
    {
      aiBone *currBone;
      aiNode *currBoneNode;
      for (int j=0; j<currMesh->mNumBones; j++)
      {
	currBone = currMesh->mBones[j];
	currBoneNode = scene->mRootNode->FindNode(currBone->mName);
	if (currBoneNode->mParent==skeletonNode)
	{
	  skeletonRootNode = currBoneNode;
	}
      }
    }
    nVertsPerMesh[i] = currMesh->mNumVertices;
    nFacesPerMesh[i] = currMesh->mNumFaces;
    totVerts += nVertsPerMesh[i];
    totFaces += nFacesPerMesh[i];
  }
  assert(skeletonRootNode);


  // Build the skeleton nodes, starting from the skeleton root node found
  ModelBone rootBone;
  unsigned int boneIdx=0, totBones=0;
  map<string, ModelBone*> name2Bone;
  _recursiveReadSkeletonNodes(skeletonRootNode, &rootBone, &boneIdx, name2Bone);
  totBones = boneIdx+1;


  // Build a single vector for vertices/faces/weights/material-indices
  // Note: the mesh is supposed to be already triangulated
  // Node: each pixels is supposed to have only one material assigned
  float *vertices = new float[totVerts*3];
  unsigned int *faces = new unsigned int[totFaces*3];
  unsigned int *vertBones = new unsigned int[totVerts*maxNBonesPerMesh];
  float *vertWeights = new float[totVerts*maxNBonesPerMesh];
  unsigned int *vertMaterials = new unsigned int[totVerts];
  memset(vertBones, 0, totVerts*sizeof(unsigned int)*maxNBonesPerMesh);
  memset(vertWeights, 0, totVerts*sizeof(float)*maxNBonesPerMesh);
  

  // Keep track of how many weights are defined per-vertex
  unsigned int *nWeightsPerVertex = new unsigned int[totVerts];
  memset(nWeightsPerVertex, 0, sizeof(unsigned int)*totVerts);

  // Fill the vectors from meshes data
  for (int i=0, vertOffset=0, facesOffset=0; i<nMeshes; i++)
  {
    aiMesh *currMesh = scene->mMeshes[meshNode->mMeshes[i]];
    unsigned int offset;

    vertOffset += (i) ? nVertsPerMesh[i-1] : 0;
    for (int v=0; v<currMesh->mNumVertices; v++)
    {
      vertices[(vertOffset+v)*3] = currMesh->mVertices[v].x;
      vertices[(vertOffset+v)*3+1] = currMesh->mVertices[v].y;
      vertices[(vertOffset+v)*3+2] = currMesh->mVertices[v].z;
      vertMaterials[vertOffset+v] = currMesh->mMaterialIndex;
    }

    for (int b=0; b<currMesh->mNumBones; b++)
    {
      aiBone *currBone = currMesh->mBones[b];
      for (int w=0; w<currBone->mNumWeights; w++)
      {
	aiVertexWeight *currWeight = &currBone->mWeights[w];
	unsigned int vertexIdx = vertOffset+currWeight->mVertexId;
	unsigned int weightOffset = vertexIdx*maxNBonesPerMesh+nWeightsPerVertex[vertexIdx];
	vertBones[weightOffset] = name2Bone[string(currBone->mName.C_Str())]->idx;
	vertWeights[weightOffset] = currWeight->mWeight;
	nWeightsPerVertex[vertexIdx]++;
      }
      // Copy the mesh-to-parent transformation matrix for the corresponding model skeleton bone
      // TODO: avoid multiple copies if the bone is present in more than one mesh
      aiMatrix4x4 *tmp = &currBone->mOffsetMatrix;
      name2Bone[string(currBone->mName.C_Str())]->meshToBoneMatrix <<
	tmp->a1, tmp->a2, tmp->a3, tmp->a4,
	tmp->b1, tmp->b2, tmp->b3, tmp->b4,
	tmp->c1, tmp->c2, tmp->c3, tmp->c4,
	tmp->d1, tmp->d2, tmp->d3, tmp->d4;
    }

    facesOffset += (i) ? nFacesPerMesh[i-1] : 0;
    for (int f=0; f<currMesh->mNumFaces; f++)
    {
      aiFace *currFace = &currMesh->mFaces[f];
      assert(currFace->mNumIndices==3);
      faces[(facesOffset+f)*3] = vertOffset+currFace->mIndices[0];
      faces[(facesOffset+f)*3+1] = vertOffset+currFace->mIndices[1];
      faces[(facesOffset+f)*3+2] = vertOffset+currFace->mIndices[2];
    }
  }


  // Load materials
  // Note: only diffuse color is handled
  totMaterials = scene->mNumMaterials;
  float *materials = new float[totMaterials*3];
  for (int m=0; m<totMaterials; m++)
  {
    aiMaterial *currMaterial = scene->mMaterials[m];
    aiColor3D diffuseColor;
    currMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

    materials[m*3] = diffuseColor.r;
    materials[m*3+1] = diffuseColor.g;
    materials[m*3+2] = diffuseColor.b;
  }
  

  // Initialize the model rendering class, filling its internal
  // data
  cgmodel.initModel(totVerts, totFaces, totBones, maxNBonesPerMesh, totMaterials);
  cgmodel.fillVertices(vertices);
  cgmodel.fillFaces(faces);
  cgmodel.fillVertexAttributes(vertBones, vertWeights, vertMaterials);
  cgmodel.fillMaterials(materials);

  // Build the skeleton hierarchy used for animation
  skeleton.init(totBones);
  _recursiveFillSkeleton(&skeleton, &rootBone);


  // Done, cleanup
  delete []nWeightsPerVertex;
  delete []nVertsPerMesh;
  delete []nFacesPerMesh;
  delete []vertices;
  delete []faces;
  delete []vertBones;
  delete []vertWeights;
  delete []vertMaterials;
  delete []materials;
  _recursiveFreeNodes(&rootBone);

  return true;
}


void _recursiveReadSkeletonNodes(aiNode *skeletonNode, ModelBone *bone, unsigned int *boneIdx, map<string, ModelBone*> &name2Bone)
{
  // Read the current node
  aiMatrix4x4 *tmp;
  bone->name = string(skeletonNode->mName.C_Str());
  bone->idx = *boneIdx;
  tmp = &skeletonNode->mTransformation;
  bone->parentRelativeMatrix << tmp->a1, tmp->a2, tmp->a3, tmp->a4,
    tmp->b1, tmp->b2, tmp->b3, tmp->b4,
    tmp->c1, tmp->c2, tmp->c3, tmp->c4,
    tmp->d1, tmp->d2, tmp->d3, tmp->d4;
  // Note: no bone length is stored within assimp nodes. Set default length to 0.
  bone->length = 0.0f;

  // Recursively read the children nodes
  bone->nChildren = skeletonNode->mNumChildren;
  for (int i=0; i<bone->nChildren; i++)
  {
    bone->children[i] = new ModelBone();
    bone->children[i]->parent = bone;
    (*boneIdx)++;
    _recursiveReadSkeletonNodes(skeletonNode->mChildren[i], bone->children[i], boneIdx, name2Bone);
  }

  // Save the name-index correspondency for current bone
  name2Bone[bone->name] = bone;
}

void  _recursiveFillSkeleton(ModelSkeleton *skeleton, ModelBone *bone)
{
  skeleton->addBone(*bone, bone->idx);
  for (int i=0; i<bone->nChildren; i++)
  {
    skeleton->addChildren(bone->idx, bone->children[i]->idx);
    _recursiveFillSkeleton(skeleton, bone->children[i]);
  }
}

void _recursiveFreeNodes(ModelBone *bone)
{
  for (int i=0; i<bone->nChildren; i++)
  {
    _recursiveFreeNodes(bone->children[i]);
    delete bone->children[i];
  }
}
