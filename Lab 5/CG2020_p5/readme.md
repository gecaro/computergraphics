# Computer Graphics: Lab 05

#### Gerard Camps i Romaguera, 193415.

___

In this lab we develop extent the lab 04 by adding textures to the mesh, this means that we do not use the material class anymore and instead we perform phong using the properties of the texture. Moreover we use a second texture to perform a normal map, this is, instead of using the normal to the model matrix, we use the values stored in a texture which has RGB values and gives much more definitions to the final render.

For this task we've reused the phong shader from the previous assignment changing some details, the snippet below is inside the main funcion of *texture.ps*:

```glsl
// obtain normal from normal map in range [0,1]
vec3 normal_m = texture2D(normal_texture, v_coord).rgb;
// transform normal vector to range [-1,1]
normal_m = normalize(normal_m * 2.0 - 1.0); 
vec3 normal = (model * vec4(normal_m, 0.0)).xyz;
```

First of all we calculate the normal in a different way, by using the values stored in the normal texture. As can be seen we get the value for each pixel, as a vector of three components, then we transform it from color range (as it is stored as a normalized color, it goes from 0 to 1) to normal space, from range [-1, 1].

Then the final value for the normal is using the model matrix as we used to do with the gl_Normal. 

The following change is regarding to the way we compute the light:

```glsl
vec3 ambient = ia;
vec3 diffuse = LdotN * id;
vec3 specular = color.w * pow(RdotV, 13.0) * is;
vec3 ip = color.xyz * (ambient + diffuse + specular);
```

As can be seen we don't use the material properties of the class anymore, instead we use the color we got from the texture, so fot the final value we just multiply the value of the color by the ambient, diffuse and specular value, because the color defined by the texture affects the three values. Note that the way of computing the specular value has changed as well. We use the w component of the color (as it is a vector of four components) because it stores the specular coefficient. We added a shininess coefficient of 13 for testing purposes.

Another approach we tried to reach was rendering another mesh with different textures. We used the files: *covid.obj*, rocks_2.tga and corona_normal.tga. The problem we faced and couldn't find solution to was to rotate the second mesh when it was rendered, and we finally dropped the idea. 

Due to personal reasons, couldn't find the time to perform the last part of the assignment, which was adapting this model to the Lab 03. 