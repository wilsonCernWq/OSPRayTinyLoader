#version 330 core

layout(location = 0) in vec3 position;
out vec2 texcoord;

void main() {
    gl_Position = vec4(position, 1.0f);
	texcoord = vec2(position.x / 2 + 0.5, position.y / 2 + 0.5);
}
