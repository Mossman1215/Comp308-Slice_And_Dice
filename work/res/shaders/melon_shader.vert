#version 120

varying float vDist;
varying vec3 vWorldPosition;
varying vec3[31] seeds;

vec3[31] buildSeeds(){
	int n = 31;
	vec3 nodes[31];
	float dlong = 2.4;  /* ~2.39996323 */
	float dz = 2.0/n;
	float long = 0;
	float z = 1 - dz/2;
	float r = 0;
	for (int k = 0; k < n; k++){
	    float r = sqrt(1-z*z);
	    vec3 raw = vec3(cos(long)*r, z, sin(long)*r);
	    vec4 condensed = gl_ModelViewMatrixInverse * vec4(raw, 0);
	    nodes[k] = vec3(condensed.x, condensed.y, condensed.z);
	    z = z - dz;
	    long = long + dlong;
	}
	return nodes;
}

void main()
{
	vec4 source = gl_ModelViewMatrixInverse * vec4(0, 1, 0, 0);
	vec4 base = vec4(gl_ModelViewMatrixInverse * vec4(gl_Vertex.x, gl_Vertex.y, gl_Vertex.z, 0));
	vDist = distance(vec3(base.x, base.y, base.z), vec3(source.x, source.y, source.z));
	vWorldPosition = vec3(base.x, base.y, base.z);
	seeds = buildSeeds();
	gl_Position = ftransform(); 
} 
