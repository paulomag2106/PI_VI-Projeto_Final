#version 330 core
in vec2 UV;
in vec4 fragmentColor;
out vec4 color;

uniform sampler2D textureSampler;

void main() {

    color = fragmentColor * texture(textureSampler, UV).rgba;

}
