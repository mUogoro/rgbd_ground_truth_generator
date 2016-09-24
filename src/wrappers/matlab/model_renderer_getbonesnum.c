#include "mex.h"

#include <model_renderer/model_renderer.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  /* Check arguments */
  if (!mxIsUint64(prhs[0]))
  {
    mexErrMsgTxt("uint64 argument expected as first argument!!!\n");  
  }
  
  /* Get the model renderer instance */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  ModelRenderer *self = (ModelRenderer*)ptr;

  /* Return the number of bones */
  mwSize outDim[2] = {1,1};
  plhs[0] = mxCreateNumericArray(2, outDim, mxUINT32_CLASS, mxREAL);
  uint32_T* outPtr = (uint32_T*)mxGetData(plhs[0]);
  *outPtr = ModelRenderer_getBonesNum(self);
}
