#version 330 core
in vec2 UV;
in vec3 fragmentColor;
out vec4 color;

uniform sampler2D textureSampler;

void main() {

    color = vec4(fragmentColor,1) * texture(textureSampler, UV).rgba;

}
