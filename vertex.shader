#version 330 core

#define M_PI 3.1415926535897932384626433832795

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 uvs;

uniform vec3 pos;
uniform vec3 scale;
uniform vec4 rotation;
uniform vec4 color;

uniform mat4 Projection;
uniform mat4 View;

out vec2 UV;
out vec4 fragmentColor;
void main() {

	// Simulate "lighting" based on camera position
	vec3 lightPos = vec3(View[0][2], View[1][2], View[2][2]);
	float cosTheta = clamp(dot(normals, lightPos),0.1f,0.9f);

	vec3 modelTransform = scale * vertexPosition_modelspace + pos;
	gl_Position = Projection * View * vec4(modelTransform,1);
	// green 0.215, 0.466, 0.262
	// brown 0.466, 0.286, 0.215
	vec3 grass = vec3(0.215, 0.466, 0.262);
	vec3 dirt = vec3(0.466, 0.286, 0.215);
	vec3 levelColor = mix(grass, dirt, vertexPosition_modelspace.z);
    fragmentColor.rgb = color.rgb * levelColor * cosTheta;
	fragmentColor.a = color.a;
	UV = uvs;

}
