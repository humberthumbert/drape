attribute vec3 position;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

varying vec3 normal;

void main() {
  normal = normalize(position);
  gl_Position = P * V * M * vec4(position, 1.0);
}
