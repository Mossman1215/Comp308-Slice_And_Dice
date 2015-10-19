#version 120

varying float vDist;
varying vec3 vWorldPosition;

void main()
{
	vec4 source = gl_ModelViewMatrixInverse * vec4(0, 0, 0, 0);
	vec4 base = vec4(gl_ModelViewMatrixInverse * vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z, 0));
	vDist = distance(vec3(base.x, base.y, base.z), vec3(source.x, source.y, source.z));
	vWorldPosition = vec3(base.x, base.y, base.z);
	gl_Position = ftransform(); 
} 
