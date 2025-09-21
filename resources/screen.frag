#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

// It needs to accept BOTH textures from your C++ code
uniform sampler2D colorTexture;
uniform sampler2D depthTexture;

void main()
{
    // Sample the depth from the depth texture
    float depth = texture(depthTexture, TexCoords).r;
    
    // Get the depth of neighboring pixels
    float depth_up    = texture(depthTexture, TexCoords + vec2(0.0, 1.0/600.0)).r;
    float depth_down  = texture(depthTexture, TexCoords - vec2(0.0, 1.0/600.0)).r;
    float depth_left  = texture(depthTexture, TexCoords - vec2(1.0/1200.0, 0.0)).r;
    float depth_right = texture(depthTexture, TexCoords + vec2(1.0/1200.0, 0.0)).r;
    
    // Calculate the difference in depth
    float depth_diff = abs(depth - depth_up) + abs(depth - depth_down) + abs(depth - depth_left) + abs(depth - depth_right);

    // If the difference is larger than a small threshold, it's an edge
    if (depth_diff > 0.0005) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0); // Draw a Black outline
    } else {
        // Otherwise, sample the final color from the color texture
        FragColor = texture(colorTexture, TexCoords);
    }
}