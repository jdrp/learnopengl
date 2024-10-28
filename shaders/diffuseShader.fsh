#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D texture_diffuse0;
};
uniform Material material;
uniform bool invertColor;
uniform bool blackAndWhite;
// for kernel, all of these must be set
uniform bool applyKernel;
uniform float increaseKernel;
uniform int screenWidth;
uniform int screenHeight;
uniform float[9] kernel;

void main()
{
    FragColor = texture(material.texture_diffuse0, TexCoords);
    if (applyKernel)
    {
        float offsetX = 1.0 / screenWidth * (1 + increaseKernel);
        float offsetY = 1.0 / screenHeight * (1 + increaseKernel);

        vec2 offsets[9] = vec2[](
        vec2(-offsetX,  offsetY), // top-left
        vec2( 0.0f,     offsetY), // top-center
        vec2( offsetX,  offsetY), // top-right
        vec2(-offsetX,  0.0f),    // center-left
        vec2( 0.0f,     0.0f),    // center-center
        vec2( offsetX,  0.0f),    // center-right
        vec2(-offsetX, -offsetY), // bottom-left
        vec2( 0.0f,    -offsetY), // bottom-center
        vec2( offsetX, -offsetY)  // bottom-right
        );

        vec3 color = vec3(0.0);
        for (int i = 0; i < 9; i++) {
            color += kernel[i] * texture(material.texture_diffuse0, TexCoords + offsets[i]).rgb;
        }

        FragColor.rgb = color;
    }

    if (invertColor)
        FragColor.rgb = 1.0 - FragColor.rgb;
    if (blackAndWhite)
        FragColor.rgb = vec3(0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b);

}