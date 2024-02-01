#version 430

layout( location = 0) in vec3 iPosition; 
layout( location = 3 ) in vec2 iTexCoord;
layout( location = 2 ) in vec3 iNormal;

layout( location = 0) uniform mat4 uProjCameraWorld;
layout( location = 1 ) uniform mat3 uNormalMatrix;


out vec2 v2fTexCoord;
out vec3 v2fNormal;

void main()
{
    v2fTexCoord = iTexCoord;
    gl_Position = uProjCameraWorld * vec4(iPosition, 1.0);
    v2fNormal = normalize(uNormalMatrix * iNormal);

}

