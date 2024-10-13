#version 330 core

in vec3 vertexNormal; // For incoming normals
in vec3 vertexFragmentPos; // For incoming fragment position
in vec2 vertexTextureCoordinate; // For incoming uv coordinates

out vec4 fragmentColor; // For outgoing cube color to the GPU

// Uniform / Global variables for object color, light color, light position, and camera/view position
uniform vec3 objectColor;
uniform vec3 lightColor0;
uniform vec3 lightPos0;
uniform vec3 lightColor1;
uniform vec3 lightPos1;
uniform vec3 viewPosition;
uniform sampler2D uTexture; // Useful when working with multiple textures

void main()
{
    /*Phong lighting model calculations to generate ambient, diffuse, and specular components*/

    //Calculate Ambient lighting*/
    float ambientStrength = 0.1f; // Set ambient or global lighting strength
    vec3 ambient = ambientStrength * lightColor0 + ambientStrength * lightColor1; // Generate ambient light color

    //Calculate Diffuse lighting*/
    vec3 norm = normalize(vertexNormal); // Normalize vectors to 1 unit
    // calculate first light component
    vec3 lightDirection0 = normalize(lightPos0 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact0 = max(dot(norm, lightDirection0), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    // calculate second light component
    vec3 lightDirection1 = normalize(lightPos1 - vertexFragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on cube
    float impact1 = max(dot(norm, lightDirection1), 0.0);// Calculate diffuse impact by generating dot product of normal and light
    
    vec3 diffuse = (impact0 * lightColor0) + (impact1 * lightColor1); // Generate diffuse light color

    //Calculate Specular lighting*/
    float specularIntensity = 0.4f; // Set specular light strength
    float highlightSize = 16.0f; // Set specular highlight size
    vec3 viewDir = normalize(viewPosition - vertexFragmentPos); // Calculate view direction
    //calculate first light component
    vec3 reflectDir0 = reflect(-lightDirection0, norm);// Calculate reflection vector
    float specularComponent0 = pow(max(dot(viewDir, reflectDir0), 0.0), highlightSize);
    //calculate second light component
    vec3 reflectDir1 = reflect(-lightDirection1, norm);// Calculate reflection vector
    float specularComponent1 = pow(max(dot(viewDir, reflectDir1), 0.0), highlightSize);

    vec3 specular = specularIntensity * specularComponent0 * lightColor0;

    // Texture holds the color to be used for all three components
    vec4 textureColor = texture(uTexture, vertexTextureCoordinate);

    // Calculate phong result
    vec3 phong = (ambient + diffuse + specular) * textureColor.xyz;

    fragmentColor = vec4(phong, 1.0); // Send lighting results to GPU
}