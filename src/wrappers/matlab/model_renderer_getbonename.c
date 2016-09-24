#include <string.h>
#include "mex.h"

#include <model_renderer/model_renderer.h>

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  /* Check arguments */
  if (!mxIsUint64(prhs[0]))
  {
    mexErrMsgTxt("uint64 argument expected as first argument!!!\n");  
  }
  if (!mxIsScalar(prhs[1]) || !mxIsNumeric(prhs[1]))
  {
    mexErrMsgTxt("Scalar argument expected as second argument!!!\n");  
  }
  
  /* Get the model renderer instance */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  ModelRenderer *self = (ModelRenderer*)ptr;

  /* Get the bone name and wrap it into a mxArray */
  unsigned int idx = (unsigned int)mxGetScalar(prhs[1]);
  const char *boneName = ModelRenderer_getBoneName(self, idx);
  if (boneName[0]==-1) mexErrMsgTxt("Bone with specified id not found!!!\n");  
  plhs[0] = mxCreateString(boneName);
}
