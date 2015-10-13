#version 330

uniform sampler2D u_scene_tex;
uniform sampler2D u_depth_tex;
in vec2 v_tex;
layout( location = 0 ) out vec4 fragcolor;
 
void main() {
 
    float z = texture(u_depth_tex, v_tex).r;
    float n = 1.0;
    float f = 30.0;
    float c = (2.0 * n) / (f + n - z * (f - n));
	fragcolor.rgb = vec3(c); 
}