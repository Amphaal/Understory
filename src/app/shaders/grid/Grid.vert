layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 textureCoordinates;

out vec2 interpolatedTextureCoordinates;

uniform mat4 u_projMatrix;

void main() {
   interpolatedTextureCoordinates = textureCoordinates;
   gl_Position = u_projMatrix * vec4(aPos, 1.0, 1.0);
}