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
  if (nlhs!=2)
  {
    mexErrMsgTxt("Expected two output arguments!!!\n");
  }

  /* Get the model renderer instance */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  ModelRenderer *self = (ModelRenderer*)ptr;
  int width = ModelRenderer_getWidth(self);
  int height = ModelRenderer_getHeight(self);
  mwSize outDim[3] = {height,width,3};
  int v,u,c;

  /* Check images size */

  /* Perform the rendering */
  unsigned char *colourBuffer = malloc(width*height*3*sizeof(unsigned char));
  unsigned short *depthBuffer = malloc(width*height*sizeof(unsigned short));
  ModelRenderer_render(self, colourBuffer, depthBuffer);

  /* Copy the rendered buffers to the matlab matrices, accounting for the different
     storing policy (column major vs row major), channels configuration for
     colours (layered channels vs. interleaved channels) and different image
     coordinates' origin (upper left vs lower left) */
  plhs[0] = mxCreateNumericArray(3, outDim, mxUINT8_CLASS, mxREAL);
  plhs[1] = mxCreateNumericArray(2, outDim, mxUINT16_CLASS, mxREAL);

  uint8_T *colourPtr = (uint8_T*)mxGetData(plhs[0]);
  for (v=0; v<height; v++)
  {
    for (u=0; u<width; u++)
    {
      for (c=0; c<3; c++)
      {
	size_t inOffset = v*width*3 + u*3 + c;
	size_t outOffset = c*width*height + u*height + (height-v-1);
	colourPtr[outOffset] = colourBuffer[inOffset];
      }
    }
  }

  uint16_T *depthPtr = (uint16_T*)mxGetData(plhs[1]);
  for (v=0; v<height; v++)
  {
    for (u=0; u<width; u++)
    {
      size_t inOffset = v*width + u;
      size_t outOffset = u*height + (height-v-1);
      depthPtr[outOffset] = depthBuffer[inOffset];
    }
  }

  /* Done */
  free(depthBuffer);
  free(colourBuffer);
}
