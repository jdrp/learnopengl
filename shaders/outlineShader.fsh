#version 420 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D stencilTexture; // The stencil buffer as a texture
uniform vec3 outlineColor;        // Color of the outline
uniform int outlineWidth;         // Outline width in pixels

uint getStencilValue(sampler2D tex, vec2 coords) {
    uint packedValue = uint(texture(tex, coords) * 0xFFFFFFFFu);
    return packedValue & 0xFFu;
}

void main()
{
    FragColor = vec4(texture(stencilTexture, TexCoords).rgb, 1.0);
    return;
    uint currentStencil = getStencilValue(stencilTexture, TexCoords);
    FragColor = vec4(vec3(currentStencil), 1.0);
    /*

    // Offset for neighboring pixels (1 pixel offset)
    vec2 texOffset = vec2(1.0 / textureSize(depthStencilTexture, 0));

    bool isOutline = false;

    // Loop over all neighboring pixels within the outlineWidth radius
    for (int x = -outlineWidth; x <= outlineWidth; x++) {
        for (int y = -outlineWidth; y <= outlineWidth; y++) {
            // Skip the current pixel
            if (x == 0 && y == 0)
                continue;

            // Check the stencil value of the neighboring pixel
            int neighborStencil = int(texture(depthStencilTexture, TexCoords + vec2(x, y) * texOffset).r * 255.0);

            // If any neighbor within the radius has a stencil value of 1, mark this pixel as part of the outline
            if (neighborStencil > 0) {
                isOutline = true;
                break;
            }
        }
        if (isOutline) break;
    }

    // Output the outline color or transparent based on the condition
    if (isOutline) {
        FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Output the outline color with full opacity
    } else {
        FragColor = vec4(0.0, currentStencil, 1.0, 1.0); // Output transparent color for non-outline areas
    }
    */
}
