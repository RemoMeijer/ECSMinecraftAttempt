#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_atlas;

void main() {
    FragColor = texture(texture_atlas, TexCoords);
}