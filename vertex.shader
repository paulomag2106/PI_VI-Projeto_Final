#version 330 core

#define M_PI 3.1415926535897932384626433832795

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 uvs;

uniform vec3 pos;
uniform vec3 scale;
uniform vec4 rotation;
uniform vec3 color;

uniform mat4 Projection;
uniform mat4 View;

out vec2 UV;
out vec3 fragmentColor;
void main() {

	vec3 modelTransform = scale * vertexPosition_modelspace + pos;
	gl_Position = Projection * View * vec4(modelTransform,1);
    fragmentColor = color;
	UV = uvs;

}
