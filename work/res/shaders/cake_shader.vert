varying float vDist;
varying float vRad;
varying vec3 vPos;

void main()
{
	vec4 source = gl_ModelViewMatrixInverse * vec4(0, -1, 0, 0);
	vec4 base = vec4(gl_ModelViewMatrixInverse * vec4(0, gl_Vertex.y, 0, 0));
	vec4 rad = vec4(gl_ModelViewMatrixInverse * vec4(gl_Vertex.x, -1, gl_Vertex.z, 0));
	vPos = vec3(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z);
	vDist = distance(vec3(base.x, base.y, base.z), vec3(source.x, source.y, source.z));
	vRad = distance(vec3(rad.x, rad.y, rad.z), vec3(source.x, source.y, source.z));
	gl_Position = ftransform(); 
} 
