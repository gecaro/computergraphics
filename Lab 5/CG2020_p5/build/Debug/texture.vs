//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;


//vars to pass to the pixel shader
varying vec2 v_coord;
varying vec3 v_wPos;
varying vec3 v_wNormal;

void main()
{	
	v_wPos = (model * gl_Vertex).xyz;
	v_wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz;

	//get the texture coordinates (per vertex) and pass them to the pixel shader
	v_coord = gl_MultiTexCoord0.xy;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(v_wPos,1.0); //output of the vertex shader
}