#include "mex.h"
#include <string.h>

#include <model_renderer/model_renderer.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  /* Check arguments */
  if (!mxIsUint64(prhs[0]))
  {
    mexErrMsgTxt("uint64 argument expected as first argument!!!\n");  
  }
  if (!mxIsChar(prhs[1]))
  {
    mexErrMsgTxt("String argument expected as second argument!!!\n");  
  }

  /* Get the model renderer instance */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  ModelRenderer *self = (ModelRenderer*)ptr;

  mwSize outDim[2] = { 1, 1 };
  /* Get the bone idx and wrap it into a mxArray */
  char boneName[1024];
  mxGetString(prhs[1], boneName, 1024);
  int idx = ModelRenderer_getBoneIdx(self, boneName);

  if (idx==-1) mexErrMsgTxt("Bone with specified name not found!!!\n");
  plhs[0] = mxCreateNumericArray(2, outDim, mxUINT32_CLASS, mxREAL);
  uint32_T* outPtr = (uint32_T*)mxGetData(plhs[0]);
  *outPtr = idx; 
}
