#include <cassert>
#include <cmath>
#include <cstring>
#include <iostream>
#include <fstream>
#include <gl_model.hpp>

#define DEFAULT_PERVERTEX_BONES (8)

#define DEFAULT_VERT_SHADER_SRC ("default_vert_shader.vs")
#define DEFAULT_FRAG_SHADER_SRC ("default_frag_shader.fs")

#define DEFAULT_ATTRIB_POSITION_NAME ("position")
#define DEFAULT_UNIFORM_BONESMAT_NAME ("bonesMat")
#define DEFAULT_UNIFORM_PROJECTIONMAT_NAME ("projectionMat")
#define DEFAULT_UNIFORM_MATERIALS_NAME ("materials")
#define DEFAULT_ATTRIB_VERTBONESID_1 ("vertBonesId1")
#define DEFAULT_ATTRIB_VERTBONESID_2 ("vertBonesId2")
#define DEFAULT_ATTRIB_VERTBONESWEIGHT_1 ("vertBonesWeight1")
#define DEFAULT_ATTRIB_VERTBONESWEIGHT_2 ("vertBonesWeight2")
#define DEFAULT_ATTRIB_MATERIALID_NAME ("materialId")

using namespace std;

#define PRINT_SHADER_COMPILATION_LOG(__shaderId) \
  {						 \
    char buffer[512];				 \
    glGetShaderInfoLog(__shaderId, 512, NULL, buffer); \
    cout << buffer << endl;			 \
  }

static char* _readShaderFile(const string &shaderFile);
void GLModel::initScene(unsigned int _width, unsigned int _height, float hFOV, float vFOV,
		       float near, float far)
{
  width = _width;
  height = _height;
  
  // Create an hidden window where the OpenGL context is attached
  // Force OpenGL 3.2, CORE profile
  glfwInit();
  
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  glfwWindowHint(GLFW_VISIBLE, 0);
  glWindow = glfwCreateWindow(width, height, "OpenGL", NULL, NULL);
  assert(glWindow);

  glfwMakeContextCurrent(glWindow);
  
  // Init OpenGL functions through GLEW
  glewExperimental = GL_TRUE;
  glewInit();

  // TODO: use glew to check if needed OpenGL functions are available
  // TODO: perform error checking on GL calls

  // Create the VBO used for storing vertices attributes
  glGenVertexArrays(1, &vboId);
  glBindVertexArray(vboId);

  // Create OpenGL buffers for rendering and setup them
  glGenFramebuffers(1, &fboId);
  glGenRenderbuffers(1, &rboColorId);
  glGenRenderbuffers(1, &rboDepthId);

  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  glBindRenderbuffer(GL_RENDERBUFFER, rboColorId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColorId);
  
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepthId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthId);

  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE);

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  

  // Build the projection matrix
  // So far suppose world and eye coordinates are the same (i.e. the camera is centered at
  // the world origin)
  // Note: the projection is perspective with respect to X and Y coordinates and orthographic
  // with respect to the Z coordinate. This allow to obtain a linear precision in the z-buffer
  // The linearity require a further per-pixel multiplication by the wc component, performed in
  // the vertex shader
  // ref: http://www.songho.ca/opengl/gl_projectionmatrix.html
  GLfloat right=near*tan(M_PI/180.0f*hFOV/2.0f), top=near*tan(M_PI/180.0f*vFOV/2.0f);
  projectionMat[0]=near/right; projectionMat[4]=0.0f;     projectionMat[8]=0.0f;                    projectionMat[12]=0.0f;
  projectionMat[1]=0.0f;       projectionMat[5]=near/top; projectionMat[9]=0.0f;                    projectionMat[13]=0.0f;
  //projectionMat[2]=0.0f;       projectionMat[6]=0.0f;     projectionMat[10]=-(far+near)/(far-near); projectionMat[14]=-2.0*far*near/(far-near);
  projectionMat[2]=0.0f;       projectionMat[6]=0.0f;     projectionMat[10]=-2.0f/(far-near);       projectionMat[14]=-(far+near)/(far-near);
  projectionMat[3]=0.0f;       projectionMat[7]=0.0f;     projectionMat[11]=-1.0f;                  projectionMat[15]=0.0f;

  // Compile the vertex and pixel shader
  const GLchar *vsSrc, *fsSrc;
  GLint status;

  vsSrc = _readShaderFile(DEFAULT_VERT_SHADER_SRC);
  fsSrc = _readShaderFile(DEFAULT_FRAG_SHADER_SRC);
  assert(vsSrc && fsSrc);

  vsId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vsId, 1, &vsSrc, NULL);
  glCompileShader(vsId);
  glGetShaderiv(vsId, GL_COMPILE_STATUS, &status);
  if (status!=GL_TRUE)
  {
    PRINT_SHADER_COMPILATION_LOG(vsId);
    assert(0);
  }

  fsId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fsId, 1, &fsSrc, NULL);
  glCompileShader(fsId);
  glGetShaderiv(fsId, GL_COMPILE_STATUS, &status);
  if (status!=GL_TRUE)
  {
    PRINT_SHADER_COMPILATION_LOG(fsId);
    assert(0);
  }

  progId = glCreateProgram();
  glAttachShader(progId, vsId);
  glAttachShader(progId, fsId);

  glBindFragDataLocation(progId, 0, "outColor");

  glLinkProgram(progId);
  glUseProgram(progId);

  // Init GL buffers. There'll be actually fill* functions 
  glGenBuffers(1, &vertBuff);
  glGenBuffers(1, &facesBuff);
  glGenBuffers(1, &vertBonesIdBuff);
  glGenBuffers(1, &vertBonesWeightBuff);
  glGenBuffers(1, &vertMaterialIdBuff);
  
  // Get vertex attributes location
  positionAttrib = glGetAttribLocation(progId, DEFAULT_ATTRIB_POSITION_NAME);
  vertBonesId1Attrib = glGetAttribLocation(progId,
					   DEFAULT_ATTRIB_VERTBONESID_1);
  vertBonesId2Attrib = glGetAttribLocation(progId,
					   DEFAULT_ATTRIB_VERTBONESID_2);
  vertBonesWeight1Attrib = glGetAttribLocation(progId,
					       DEFAULT_ATTRIB_VERTBONESWEIGHT_1);
  vertBonesWeight2Attrib = glGetAttribLocation(progId,
					       DEFAULT_ATTRIB_VERTBONESWEIGHT_2);  
  vertMaterialIdAttrib = glGetAttribLocation(progId,
					     DEFAULT_ATTRIB_MATERIALID_NAME);

  // Get the uniform locations and load the already known projection matrix
  projectionMatUnif = glGetUniformLocation(progId,
					   DEFAULT_UNIFORM_PROJECTIONMAT_NAME);
  glUniformMatrix4fv(projectionMatUnif, 1, GL_FALSE, projectionMat);

  bonesMatUnif = glGetUniformLocation(progId,
				      DEFAULT_UNIFORM_BONESMAT_NAME);
  materialsUnif = glGetUniformLocation(progId,
				       DEFAULT_UNIFORM_MATERIALS_NAME);


  // Finally, set the last OpenGL stuff
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  delete []vsSrc;
  delete []fsSrc;

  // Done
}

void GLModel::initModel(unsigned int _nVertices,
			unsigned int _nFaces,
			unsigned int _nBones,
			unsigned int _maxBonesPerVert,
			unsigned int _nMaterials)
{
  nVertices = _nVertices;
  nFaces = _nFaces;
  nBones = _nBones;
  maxBonesPerVert = _maxBonesPerVert;
  nMaterials = _nMaterials;
  assert(nBones<=24 && nMaterials<=24);
}

void GLModel::fillVertices(const float *vertices)
{
  glfwMakeContextCurrent(glWindow);
  glBindBuffer(GL_ARRAY_BUFFER, vertBuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*nVertices*3, vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(positionAttrib);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void GLModel::fillFaces(const unsigned int *faces)
{
  glfwMakeContextCurrent(glWindow);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesBuff);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*nFaces*3, faces, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


void GLModel::fillVertexAttributes(const unsigned int *bonesIdx, const float *weights,
				  const unsigned int *materialsIdx)
{
  unsigned int *bonesIdxPtr = NULL;
  float *weightsPtr = NULL;

  if (maxBonesPerVert==DEFAULT_PERVERTEX_BONES)
  {
    // Suppose the number of bones-per-vertex is 8,
    // i.e. the offset between consecutive elements within the arrays is 8
    bonesIdxPtr = (unsigned int*)bonesIdx;
    weightsPtr = (float*)weights;
  }
  else
  {
    // TODO: modelle cases when the number of bones per vertex is less than 8
    //assert(0);

    bonesIdxPtr = new unsigned int[nVertices*DEFAULT_PERVERTEX_BONES];
    weightsPtr = new float[nVertices*DEFAULT_PERVERTEX_BONES];
    for (int i=0; i<nVertices; i++)
    {
      if (maxBonesPerVert>DEFAULT_PERVERTEX_BONES)
      {
	float weightsSum = 0.0f;

	memcpy(bonesIdxPtr+i*DEFAULT_PERVERTEX_BONES, bonesIdx+i*maxBonesPerVert,
	       sizeof(unsigned int)*DEFAULT_PERVERTEX_BONES);
	memcpy(weightsPtr+i*DEFAULT_PERVERTEX_BONES, weights+i*maxBonesPerVert,
	       sizeof(float)*DEFAULT_PERVERTEX_BONES);
	
	// Normalize
	for (int j=0; j<DEFAULT_PERVERTEX_BONES; j++) weightsSum+=*(weights+i*maxBonesPerVert+j);
	for (int j=0; j<DEFAULT_PERVERTEX_BONES; j++) weightsPtr[i*DEFAULT_PERVERTEX_BONES+j]/=weightsSum;
      }
      else
      {
	memcpy(bonesIdxPtr+i*DEFAULT_PERVERTEX_BONES, bonesIdx+i*maxBonesPerVert,
	       sizeof(unsigned int)*maxBonesPerVert);
	memcpy(weightsPtr+i*DEFAULT_PERVERTEX_BONES, weights+i*maxBonesPerVert,
	       sizeof(float)*maxBonesPerVert);
	// Fill remaining weights with zeros
	memset(bonesIdxPtr+i*DEFAULT_PERVERTEX_BONES+maxBonesPerVert, 0,
	       sizeof(unsigned int)*(DEFAULT_PERVERTEX_BONES-maxBonesPerVert));
	memset(weightsPtr+i*DEFAULT_PERVERTEX_BONES+maxBonesPerVert, 0,
	       sizeof(float)*(DEFAULT_PERVERTEX_BONES-maxBonesPerVert));
      }
    }
    
  }

  // Setup and bind per-vertex bones index
  glfwMakeContextCurrent(glWindow);
  glBindBuffer(GL_ARRAY_BUFFER, vertBonesIdBuff);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLuint)*nVertices, bonesIdxPtr, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vertBonesId1Attrib);
  glVertexAttribIPointer(vertBonesId1Attrib, 4, GL_UNSIGNED_INT, 8*sizeof(GLuint), 0);
  glEnableVertexAttribArray(vertBonesId2Attrib);
  glVertexAttribIPointer(vertBonesId2Attrib, 4, GL_UNSIGNED_INT, 8*sizeof(GLuint),
  			(void*)(sizeof(GLuint)*4));


  // Setup and bind per-vertex bones weight
  glBindBuffer(GL_ARRAY_BUFFER, vertBonesWeightBuff);
  glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat)*nVertices, weightsPtr, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vertBonesWeight1Attrib);
  glVertexAttribPointer(vertBonesWeight1Attrib, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), 0);
  glEnableVertexAttribArray(vertBonesWeight2Attrib);
  glVertexAttribPointer(vertBonesWeight2Attrib, 4, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat),
  			(void*)(sizeof(GLfloat)*4));

  // Setup per vertex material index
  glBindBuffer(GL_ARRAY_BUFFER, vertMaterialIdBuff);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLuint)*nVertices, materialsIdx, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vertMaterialIdAttrib);
  glVertexAttribIPointer(vertMaterialIdAttrib, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);

  if (maxBonesPerVert!=DEFAULT_PERVERTEX_BONES)
  {
    delete []bonesIdxPtr;
    delete []weightsPtr;
  }
}

void GLModel::fillMaterials(const float *materials)
{
  glfwMakeContextCurrent(glWindow);

  // Normalize materials
  glUniform3fv(materialsUnif, nMaterials, materials);
}


void GLModel::updateBones(const float *bones)
{
  glfwMakeContextCurrent(glWindow);
  glUniformMatrix4fv(bonesMatUnif, nBones, GL_FALSE, bones);
}


void GLModel::render(void *buffer, void *zbuffer)
{
  glfwMakeContextCurrent(glWindow);
  glBindFramebuffer(GL_FRAMEBUFFER, fboId);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindVertexArray(vboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesBuff);
  glDrawElements(GL_TRIANGLES, nFaces*3, GL_UNSIGNED_INT, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glFinish();
  
  if (buffer)
  {
    //glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
  }
	       
  if (zbuffer)
  {
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, zbuffer);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GLModel::clean()
{
  glfwMakeContextCurrent(glWindow);
  glDeleteVertexArrays(1, &vboId);
  glDeleteBuffers(1, &vertBuff);
  glDeleteBuffers(1, &facesBuff);
  glDeleteBuffers(1, &vertBonesIdBuff);
  glDeleteBuffers(1, &vertBonesWeightBuff);
  glDeleteBuffers(1, &vertMaterialIdBuff);

  glDeleteShader(fsId);
  glDeleteShader(vsId);

  glDeleteRenderbuffers(1, &rboDepthId);
  glDeleteRenderbuffers(1, &rboColorId);
  glDeleteFramebuffers(1, &fboId);

  glfwDestroyWindow(glWindow);
  glfwTerminate();
}


char* _readShaderFile(const string &shaderFile)
{
  char *src = NULL;
  ifstream _file(shaderFile.c_str(), ifstream::in|ifstream::binary);
  if (_file)
  {
    int fileSize;

    _file.seekg(0, _file.end);
    fileSize = _file.tellg();
    fileSize++;
    _file.seekg(0, _file.beg);

    src = new char[fileSize];
    _file.read(src, fileSize);
    src[fileSize-1] = 0;
    _file.close();
  }
  
  return src;
}
