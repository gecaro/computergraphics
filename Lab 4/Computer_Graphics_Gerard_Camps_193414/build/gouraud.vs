//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;
// point where the light is located
uniform vec3 lightpos;
// position of the eye
uniform vec3 eyepos;
// ambient reflection of the material
uniform vec3 ka;
// reflected diffuse light of the material
uniform vec3 kd;
// reflected specular light of the material
uniform vec3 ks;
// shininess factor
uniform float alpha;
// ambient light in the scene 
uniform vec3 ia;
// amount of diffuse light 
uniform vec3 id;
// amount of specular light 
uniform vec3 is;



//vars to pass to the pixel shader
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec3 ip;

//here create uniforms for all the data we need here

void main()
{	
	//convert local coordinate to world coordinates
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz;
	//convert local normal to world coordinates
	vec3 N = (model * vec4( gl_Normal.xyz, 0.0)).xyz;
	vec3 L = normalize(lightpos - wPos);
	vec3 V = normalize(wPos - eyepos);
	vec3 R = normalize(reflect(L, N));
	//pass them to the pixel shadeer interpolated

	//in GOURAUD compute the color here and pass it to the pixel shader
	//...
	float LdotN = clamp(dot(L, N), 0.0, 1.0);
	float RdotV = clamp(dot(R, V), 0.0, 1.0);

	vec3 ambient = ka * ia;
	vec3 diffuse = kd * LdotN * id;
	vec3 specular = ks * pow(RdotV, alpha) * is;
	ip = ambient + diffuse + specular;

	v_wPos = wPos;
	v_wNormal = N;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}