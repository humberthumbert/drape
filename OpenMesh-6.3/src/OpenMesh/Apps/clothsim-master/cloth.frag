uniform vec3 color;
uniform vec3 back_color;
varying vec3 normal;

void main() {
  vec3 act_normal = gl_FrontFacing ? normal : -normal;
  float attenuation = max(dot(act_normal, vec3(0.0, 1.0, 0.0)) , 0.0);
  attenuation += 0.2;
  vec3 act_color = gl_FrontFacing ? color : back_color;
  gl_FragColor = vec4(act_color, 1) * attenuation;
}
