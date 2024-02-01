#version 430

//these are the inputs to the vertex shader
layout( location = 0) in vec2 iPosition; // location is the position the item is in the VAO
layout( location = 1) in vec4 iColor;


//the shader has one output, the vertex colour.
out vec4 v2fColor; // v2f = vertex to fragment

//the main method is run for each vertex that is processed
void main()
{
    //copy intput color to output color attribute
    v2fColor = iColor;

    //gl_Position is an inbuilt position attribute
    //this attribute is the clip space position which is always vec4
    //we set z to 0, z = zero is in the middle of the openGL clip space
    //w = 1, since this is a point and not a direction

    gl_Position = vec4(iPosition.xy, 0.0, 1.0);
}
