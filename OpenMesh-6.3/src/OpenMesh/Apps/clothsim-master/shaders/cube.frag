uniform vec3 color;

varying vec3 normal;

void main() {
  float attenuation = max(dot(normal, vec3(0.0, 1.0, 0.0)) , 0.0);
  attenuation += 0.2;
  gl_FragColor = vec4(color,1) * attenuation;
}
