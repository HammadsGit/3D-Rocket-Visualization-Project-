#version 430

layout( location = 0) in vec3 iPosition; 
layout( location = 1) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;

layout( location = 0) uniform mat4 uProjection;
layout( location = 1) uniform mat4 uCamera2World;
layout( location = 5) uniform mat4 uModel2World;
layout( location = 6 ) uniform mat3 uNormalMatrix;

out vec3 v2fColor; 
out vec3 v2fNormal;
out vec3 vertPosition;

void main()
{
    v2fColor = iColor;
    vec4 homogenousCoords = uModel2World * vec4(iPosition, 1.0);

    // Convert to 3D vector by taking the xyz components
    vertPosition = homogenousCoords.xyz;
    gl_Position = uProjection * uCamera2World * uModel2World * vec4(iPosition, 1.0);
    v2fNormal = (uNormalMatrix * iNormal);
}
