layout (location = 0) in vec2 aPos;

uniform mat4 u_projMatrix;

void main() {
   gl_Position = u_projMatrix * vec4(aPos, 0.0, 1.0);
}