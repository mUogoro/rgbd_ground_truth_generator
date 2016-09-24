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
  /*
  if (!mxIsUint8(prhs[1]))
  {
    mexErrMsgTxt("Unsigned int 8 array expected as second argument!!!\n");
  }
  if (!mxIsUint16(prhs[2]))
  {
    mexErrMsgTxt("Unsigned int 16 array expected as third argument!!!\n");
  }
  */
  if (nlhs!=2)
  {
    mexErrMsgTxt("Expected two output arguments!!!\n");
  }

  /* Get the model renderer instance */
  uint64_T ptr = (uint64_T)mxGetScalar(prhs[0]);
  ModelRenderer *self = (ModelRenderer*)ptr;
  int width = ModelRenderer_getWidth(self);
  int height = ModelRenderer_getHeight(self);

  /* Check images size */
  /*
  mwSize nDimImg = mxGetNumberOfDimensions(prhs[1]);
  const mwSize *dimImg = mxGetDimensions(prhs[1]);
  if (nDimImg!=3 || dimImg[0]!=height || dimImg[1]!=width || dimImg[2]!=3)
  {
    char error[1024];
    sprintf(error, "Expected matrix of size [%d %d 3] as second argument\n",
	    height, width);
    mexErrMsgTxt(error);
  }

  nDimImg = mxGetNumberOfDimensions(prhs[2]);
  dimImg = mxGetDimensions(prhs[2]);
  if (nDimImg!=2 || dimImg[0]!=height || dimImg[1]!=width)
  {
    char error[1024];
    sprintf(error, "Expected matrix of size [%d %d] as third argument\n",
	    height, width);
    mexErrMsgTxt(error);
  }
  */

  /* Perform the rendering */
  unsigned char *colourBuffer = malloc(width*height*3*sizeof(unsigned char));
  unsigned short *depthBuffer = malloc(width*height*sizeof(unsigned short));
  ModelRenderer_render(self, colourBuffer, depthBuffer);

  /* Copy the rendered buffers to the matlab matrices, accounting for the different
     storing policy (column major vs row major), channels configuration for
     colours (layered channels vs. interleaved channels) and different image
     coordinates' origin (upper left vs lower left) */
  mwSize outDim[3] = {height,width,3};
  plhs[0] = mxCreateNumericArray(3, outDim, mxUINT8_CLASS, mxREAL);
  plhs[1] = mxCreateNumericArray(2, outDim, mxUINT16_CLASS, mxREAL);

  uint8_T *colourPtr = (uint8_T*)mxGetData(plhs[0]);
  for (int v=0; v<height; v++)
  {
    for (int u=0; u<width; u++)
    {
      for (int c=0; c<3; c++)
      {
	size_t inOffset = v*width*3 + u*3 + c;
	size_t outOffset = c*width*height + u*height + (height-v-1);
	colourPtr[outOffset] = colourBuffer[inOffset];
      }
    }
  }

  uint16_T *depthPtr = (uint16_T*)mxGetData(plhs[1]);
  for (int v=0; v<height; v++)
  {
    for (int u=0; u<width; u++)
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
