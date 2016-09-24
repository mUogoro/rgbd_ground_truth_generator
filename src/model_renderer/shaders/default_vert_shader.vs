#version 140


// Chained transformation at each skeleton bone
// TODO: allow arbitrary number of bones
uniform mat4 bonesMat[24];

// TODO: allow uniform number of materials
uniform vec3 materials[24];

uniform mat4 projectionMat;


in vec3 position;
in ivec4 vertBonesId1;
in ivec4 vertBonesId2;
in vec4 vertBonesWeight1;
in vec4 vertBonesWeight2;
in uint materialId;

out vec3 vertColor;

void main()
{       
    gl_Position = vec4(position, 1.0);

    
    gl_Position = 
      vertBonesWeight1[0] * bonesMat[vertBonesId1[0]]*gl_Position +
      vertBonesWeight1[1] * bonesMat[vertBonesId1[1]]*gl_Position +
      vertBonesWeight1[2] * bonesMat[vertBonesId1[2]]*gl_Position +
      vertBonesWeight1[3] * bonesMat[vertBonesId1[3]]*gl_Position +
      
      vertBonesWeight2[0] * bonesMat[vertBonesId2[0]]*gl_Position +
      vertBonesWeight2[1] * bonesMat[vertBonesId2[1]]*gl_Position +
      vertBonesWeight2[2] * bonesMat[vertBonesId2[2]]*gl_Position +
      vertBonesWeight2[3] * bonesMat[vertBonesId2[3]]*gl_Position;
    
    gl_Position = projectionMat*gl_Position;
    gl_Position.z *= gl_Position.w;
    vertColor = materials[materialId];
}
