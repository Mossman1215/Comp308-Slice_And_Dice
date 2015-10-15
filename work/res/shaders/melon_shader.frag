#version 120
#define pi = 3.141592653589;

uniform float age;
uniform float decayRate;
uniform float radius;
uniform sampler2D seeds;
varying vec3 vWorldPosition;
varying float vDist;
int seedCount = 300;

vec3 getSeed(int instance, int count){
	int n = count;
	float dlong = 2.4;  /* ~2.39996323 */
	float dz = 2.0/n;
	float my_long = 0;
	float z = 1 - dz/2;
	float r = 0;
	my_long = (my_long + (dlong * instance));
	z = z - (dz * instance);
	r = sqrt(1-z*z);
	float modifier = 0.9 + ((sin((r * 12.3 + z * 94.8 + my_long * 123.75) * 436.4)) * 0.1);
	vec3 raw = vec3(cos(my_long)*r, z, sin(my_long)*r) * 0.6 * modifier * radius;
	vec4 condensed = gl_ModelViewMatrixInverse * vec4(raw, 0);
	return vec3(condensed.x, condensed.y, condensed.z);
}

void main()
{
	float dist = vDist;
	float choice = dist/radius;
	vec4 base = vec4(0, 0, 0, 0);
	bool seed = false;
	for (int i = 0; i < seedCount; i++){
		if (distance(vWorldPosition, getSeed(i, seedCount)) < 0.04){
			seed = true;
			base = vec4(0.1, 0.05, 0.05, 1);
			break;
		}
	}
	if (!seed){
		if (choice > 0.9){
			base = vec4(0, 0.6, 0.2, 1);
		} else if (choice > 0.89){
			base = vec4(0.7, 0.95, 0.9, 1);
		} else if (choice > 0.8){
			base = vec4(0.9, 1, 0.98, 1);
		} else if (choice > 0.75){
			base = vec4(1, 0.9, 1, 1);
		} else if (choice > 0.7){
			base = vec4(1, 0.3, 0.6, 1);
		} else if (choice > 0.6){
			base = vec4(1, 0.25, 0.5, 1);
		} else {
			base = vec4(0.9, 0.2, 0.4, 1);
		}
	}
	gl_FragColor = base;
}
