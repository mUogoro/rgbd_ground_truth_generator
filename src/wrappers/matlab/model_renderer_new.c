#include "mex.h"

#include <model_renderer/model_renderer.h>



void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  /* Arguments check */
  if (nrhs!=7)  mexErrMsgTxt("Wrong number of arguments (seven expected)\n");
  if (!mxIsChar(prhs[2])) mexErrMsgTxt("String expected as third argument\n");
  
  /* Get the argument values: 
    - rendering width and height */
  unsigned int width, height;
  width = (unsigned int)mxGetScalar(prhs[0]);
  height = (unsigned int)mxGetScalar(prhs[1]);
  /* - horizontal and vertical field of view */
  float hFOV, vFOV;
  hFOV = (float)mxGetScalar(prhs[3]);
  vFOV = (float)mxGetScalar(prhs[4]);
  /* - near and far clipping planes */
  float near, far;
  near = (float)mxGetScalar(prhs[5]);
  far = (float)mxGetScalar(prhs[6]);
  /* - model model path */
  char modelPath[1024];
  mxGetString(prhs[2], modelPath, 1024);

  /* Create a new ModelRenderer instance, init and return it to matlab as an
     integer value */
  ModelRenderer *self = ModelRenderer_new();
  ModelRenderer_init(self, width, height, modelPath, hFOV, vFOV, near, far);

  mwSize outDim[2] = {1,1};
  plhs[0] = mxCreateNumericArray(2, outDim, mxUINT64_CLASS, mxREAL);

  uint64_T* ptr = (uint64_T*)mxGetData(plhs[0]);
  *ptr = (uint64_T)self;
}
