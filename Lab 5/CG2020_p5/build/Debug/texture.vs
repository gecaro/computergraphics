//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;
uniform sampler2D normal_texture;


//vars to pass to the pixel shader
varying vec2 v_coord;
varying vec3 v_wPos;
varying vec3 v_wNormal;

void main()
{	

	// obtain normal from normal map in range [0,1]
    
    // transform normal vector to range [-1,1]
	v_wPos = (model * gl_Vertex).xyz;
	v_wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz;

	//get the texture coordinates (per vertex) and pass them to the pixel shader
	v_coord = gl_MultiTexCoord0.xy;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(v_wPos,1.0); //output of the vertex shader
}