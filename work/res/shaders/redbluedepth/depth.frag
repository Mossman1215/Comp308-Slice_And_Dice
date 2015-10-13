uniform sampler2D sceneSampler; // 0
uniform sampler2D depthSampler; // 1
 
float LinearizeDepth(vec2 uv)
{
  float n = 1.0; // camera z near
  float f = 100.0; // camera z far
  float z = texture2D(depthSampler, uv).x;
  return (2.0 * n) / (f + n - z * (f - n));	
}
void main() 
{ 
  vec2 uv = gl_TexCoord[0].xy;
  //vec4 sceneTexel = texture2D(sceneSampler, uv);
  float d;
  if (uv.x < 0.5) // left part
    d = LinearizeDepth(uv);
  else // right part
    d = texture2D(depthSampler, uv).x;
  gl_FragColor.rgb = vec4(d, d, d, 1.0);
}