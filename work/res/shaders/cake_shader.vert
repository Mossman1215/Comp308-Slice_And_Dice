varying float vDist;
varying vec3 vPos;
void main()
{
	vec4 source = gl_ModelViewMatrixInverse * vec4(0, -2, 0, 0);
	vec4 base = vec4(gl_ModelViewMatrixInverse * vec4(0, gl_Vertex.y, 0, 0));
	vPos = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z);
	vDist = distance(vec3(base.x, base.y, base.z), vec3(source.x, source.y, source.z));
	gl_Position = ftransform(); 
} 
