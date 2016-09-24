#include "mex.h"
#include <model_renderer/model_renderer.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  /* Check argument type */
  if (!mxIsUint64(prhs[0]))
  {
    mexErrMsgTxt("uint64 argument expected as first argument!!!\n");  
  }

  /*
  uint64_T ptr = *(uint64_T*)mxGetData(prhs[0]);
  */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  
  ModelRenderer *self = (ModelRenderer*)ptr;
  ModelRenderer_delete(self);
  self = NULL;
}
