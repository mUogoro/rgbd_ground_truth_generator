#include <stdio.h>
#include <stdlib.h>
#include "mex.h"
#include <model_renderer/model_renderer.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  /* Check arguments */
  if (!mxIsUint64(prhs[0]))
  {
    mexErrMsgTxt("uint64 argument expected as first argument!!!\n");  
  }
  if (!mxIsDouble(prhs[1]) && !mxIsSingle(prhs[1]))
  {
    mexErrMsgTxt("Double or single array expected as second argument!!!\n");
  }

  /* Get the model renderer instance */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  ModelRenderer *self = (ModelRenderer*)ptr;

  /* Check pose matrix size */
  mwSize nDimPoseMatrix = mxGetNumberOfDimensions(prhs[1]);
  const mwSize *dimPoseMatrix = mxGetDimensions(prhs[1]);
  int nBones = ModelRenderer_getBonesNum(self);
  if ((nDimPoseMatrix!=2 && nDimPoseMatrix!=3) ||      // Single 4x4 matrix or list of 4x4 matrices
      dimPoseMatrix[0]!=4 || dimPoseMatrix[1]!=4 ||    // Matrices must be 4x4
      (nDimPoseMatrix==3 && dimPoseMatrix[2]!=nBones)) // If a list of matrices is passed, it must have nBones elements
  {
    char error[1024];
    sprintf(error, "Expected matrix of size [4 4 %d] as second argument\n",
	    nBones);
    mexErrMsgTxt(error);
  }

  /* Get the pose matrix, convert it to float if necessary */
  void *poseMatrixPtr = mxGetData(prhs[1]);
  float *poseMatrix;
  if (mxIsDouble(prhs[1]))
  {
    int i=0;
    poseMatrix = malloc(4*4*nBones*sizeof(float));
    for (; i<4*4*nBones; i++) poseMatrix[i] = ((double*)poseMatrixPtr)[i];
  }
  else
  {
    poseMatrix = (float*)poseMatrixPtr;
  }

  /* Finally, set the pose */
  ModelRenderer_setPose(self, poseMatrix);

  if (mxIsDouble(prhs[1])) free(poseMatrix);
}
