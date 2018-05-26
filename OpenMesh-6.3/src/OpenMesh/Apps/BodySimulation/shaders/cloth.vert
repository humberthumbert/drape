attribute vec3 position;
attribute vec3 adjacent_left;
attribute vec3 adjacent_down;
attribute vec3 adjacent_up;
attribute vec3 adjacent_right;

varying vec3 normal;

uniform mat4 V;
uniform mat4 P;

void main() {
  vec3 left = adjacent_left - position;
  vec3 right = adjacent_right - position;
  vec3 up = adjacent_up - position;
  vec3 down = adjacent_down - position;

  vec3 temp; 
  temp += cross(up, left);
  temp += cross(left, down);
  temp += cross(down, right);
  temp += cross(right, up);

  normal = normalize(temp);
  gl_Position = P * V * vec4(position, 1.0);
}
