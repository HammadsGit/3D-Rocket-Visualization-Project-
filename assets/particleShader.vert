#version 430

layout( location = 0) in vec3 iPosition; 
layout( location = 1 ) in vec3 iColor;
layout( location = 2 ) in vec3 iNormal;


layout( location = 0) uniform mat4 uProjection;
layout( location = 1) uniform mat4 uCamera2World;
layout( location = 5) uniform mat4 uModel2World;
layout( location = 6 ) uniform mat3 uNormalMatrix;


// move particle by this much
//layout( location = 5) uniform float offset;

out vec3 v2fColor; 
out vec3 v2fNormal;

void main()
{
    // scale particule quad
    //float scale = 10.f;
    v2fColor = iColor;
    gl_Position = uProjection * uCamera2World * uModel2World * vec4(iPosition, 1.0);;
    v2fNormal = normalize(uNormalMatrix * iNormal);

}

 