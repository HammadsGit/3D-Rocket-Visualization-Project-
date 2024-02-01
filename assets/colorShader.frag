#version 430

in vec3 v2fColor;
in vec3 v2fNormal;
in vec3 vertPosition;
//in vec3 vertPosition;
layout( location = 2 ) uniform vec3 landMassLight; 
layout( location = 3 ) uniform vec3 uLightDiffuse; 
layout( location = 4 ) uniform vec3 uSceneAmbient;
layout( location = 7 ) uniform vec3 uSpecular;
layout(location = 8) uniform vec3 lightPositions[3];
layout(location = 11) uniform vec3 lightColors[3];
layout(location = 15) uniform vec3 vertPositionOutside[3];
layout( location = 0 ) out vec3 oColor;

void main()
{
    //get landmass light
    
    //original colour
   // oColor = v2fColor;
    float shininess = 32.0;
    float lightPower = 2.0;
    vec3 color;  
    vec3 landmassColour;

    vec3 normal = normalize(v2fNormal);
    float landmassnDotL = max( 0.0, dot( normal, landMassLight ) );
    landmassColour = (  uSceneAmbient + landmassnDotL * uLightDiffuse ) * v2fColor;
    for (int i = 0; i < 3; ++i) 
    {
        
        vec3 lightPos = lightPositions[i];
        vec3 uLightDir = lightPos - vertPosition;

        float distance = length(uLightDir);
        distance = distance * distance;

        vec3 lightDir = normalize(uLightDir);

        float nDotL = max( 0.0, dot( lightDir, normal ) );
        float specular = 0.0;

        if(nDotL > 0.0) {
            vec3 viewDir = normalize(-vertPosition);
            vec3 halfDir = normalize(lightDir + viewDir);
            float specAngle = max(dot(halfDir, normal), 0.0);
            specular = pow(specAngle, shininess);
        }
       
        color += ((uSceneAmbient +
                         uLightDiffuse * nDotL * lightColors[i] * lightPower) / distance +
                         (uSpecular * specular * lightColors[i] * lightPower) / distance) ;
    }
    // shading + landmass shading
    oColor =  color + landmassColour;
}


