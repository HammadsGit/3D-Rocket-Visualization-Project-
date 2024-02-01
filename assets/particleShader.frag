#version 430

in vec3 v2fColor;
in vec3 v2fNormal;

layout(location = 0) out vec3 oColor;

void main()
{
    
    vec3 normal = normalize(v2fNormal); 
    
    oColor = v2fColor;

}