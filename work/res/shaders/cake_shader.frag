#version 120

uniform float age;
uniform float decayRate;
uniform float height;
varying float vDist;
varying vec3 vPos;

void main()
{
	float dist = vDist;
	float choice = dist/height;
	float remainder = choice - floor(choice);
	vec4 base = vec4(0, 0, 0, 0);
	if (remainder > 0.98){
		base = vec4(0.85, 0.75, 0.45, 1);
	} else if (remainder > 0.95){
		base = vec4(0.9, 0, 0.6, 1);
	} else if (remainder > 0.9){
		base = vec4(0.8, 0, 0.3, 1);
	} else if (remainder > 0.85){
		base = vec4(0.6, 0, 0.2, 1);
	} else if (remainder > 0.75){
		base = vec4(0.83, 0.73, 0.43, 1);
	} else if (remainder > 0.5){
		base = vec4(0.85, 0.75, 0.45, 1);
	} else {
		base = vec4(0.9, 0.8, 0.5, 1);
	}
	
	gl_FragColor = base;
}
