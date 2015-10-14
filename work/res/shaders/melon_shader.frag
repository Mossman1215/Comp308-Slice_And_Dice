#version 120
#define pi = 3.141592653589;

uniform float age;
uniform float decayRate;
uniform float radius;
varying vec3 vWorldPosition;
varying float vDist;
varying vec3[31] seeds;

void main()
{
	float dist = vDist;
	float choice = dist/radius;
	vec4 base = vec4(0, 0, 0, 0);
	bool seed = false;
	for (int i = 0; i < seeds.length; i++){
		if (distance(vWorldPosition, seeds[i]) < 0.1){
			seed = true;
			base = vec4(0, 0, 0, 1);
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
