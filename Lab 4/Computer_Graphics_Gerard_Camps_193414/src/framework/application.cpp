#include "application.h"
#include "utils.h"
#include "includes.h"
#include "utils.h"

#include "image.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "camera.h"
#include "material.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Mesh* mesh1 = NULL;
Shader* shader = NULL;

//might be useful...
Material* material = NULL;
Light* light1 = NULL;
Light* light2 = NULL;
Shader* phong_shader = NULL;
Shader* gouraud_shader = NULL;
bool light2_isOn = true;

Vector3 ambient_light(0.1,0.2,0.3); //here we can store the global ambient light of the scene

float angle = 0;

Application::Application(const char* caption, int width, int height)
{
	this->window = createWindow(caption, width, height);

	// initialize attributes
	// Warning: DO NOT CREATE STUFF HERE, USE THE INIT 
	// things create here cannot access opengl
	int w,h;
	SDL_GetWindowSize(window,&w,&h);

	this->window_width = w;
	this->window_height = h;
	this->keystate = SDL_GetKeyboardState(NULL);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;
	
	//here we create a global camera and set a position and projection properties
	camera = new Camera();
	camera->lookAt(Vector3(0,20,20),Vector3(0,10,0),Vector3(0,1,0));
	camera->setPerspective(60,window_width / window_height,0.1,10000);

	//then we load a mesh
	mesh = new Mesh();
    mesh1 = new Mesh();
	if( !mesh->loadOBJ( "lee.obj" ) )
		std::cout << "FILE Lee.obj NOT FOUND " << std::endl;


	//we load one or several shaders...
    gouraud_shader = Shader::Get( "gouraud.vs", "gouraud.ps" );
	phong_shader = Shader::Get( "phong.vs", "phong.ps" );
    shader = phong_shader;

	//load your Gouraud and Phong shaders here and stored them in some global variables
	//...

	//CODE HERE:
	//create a light (or several) and and some materials
	//...
    light1 = new Light();
    light1->position = Vector3(-100,50,10);
    light2 = new Light();
    material = new Material();
    
}

//render one frame
void Application::render(void)
{
	//update the aspect of the camera acording to the window size
	camera->aspect = window_width / window_height;
	camera->updateProjectionMatrix();
	//Get the viewprojection matrix from our camera
	Matrix44 viewprojection = camera->getViewProjectionMatrix();

	//set the clear color of the colorbuffer as the ambient light so it matches
	glClearColor(ambient_light.x, ambient_light.y, ambient_light.z, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear framebuffer and depth buffer 
	glEnable( GL_DEPTH_TEST ); //enable depth testing for occlusions
	glDepthFunc(GL_LEQUAL); //Z will pass if the Z is LESS or EQUAL to the Z of the pixel

	//choose a shader and enable it
	shader->enable();

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0,0,0); //example of translation
	model_matrix.rotate(angle, Vector3(0, 1, 0));
	shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader

	//CODE HERE: pass all the info needed by the shader to do the computations
	//send the material and light uniforms to the shader
	//...
    shader->setVector3("lightpos", light1->position);
    shader->setVector3("eyepos", camera->eye);
    shader->setVector3("ka", material->ambient);
    shader->setVector3("kd", material->diffuse);
    shader->setVector3("ks", material->specular);
    shader->setFloat("alpha", material->shininess);
    shader->setVector3("ia", Vector3(.07, .07, .11));
    shader->setVector3("id", light1->diffuse_color);
    shader->setVector3("is", light1->specular_color);
	//do the draw call into the GPU
    glDisable( GL_BLEND );
	mesh->render(GL_TRIANGLES);
    // set the second model matrix for the second mesh
    model_matrix.setIdentity();
    model_matrix.translate(30,0,0);
    model_matrix.rotate(angle, Vector3(0, 1, 0));
    shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
    shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
    mesh->render(GL_TRIANGLES);
    
    if (light2_isOn){
        model_matrix.setIdentity();
        model_matrix.translate(0,0,0); //example of translation
        model_matrix.rotate(angle, Vector3(0, 1, 0));
        shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
        shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
        shader->setVector3("lightpos", light2->position);
        shader->setVector3("ia", Vector3(.07, .07, .11));
        shader->setVector3("id", light2->diffuse_color);
        shader->setVector3("is", light2->specular_color);
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE );
        mesh->render(GL_TRIANGLES);
        model_matrix.setIdentity();
        model_matrix.translate(30,0,0); //example of translation
        model_matrix.rotate(angle, Vector3(0, 1, 0));
        shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
        shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
        mesh->render(GL_TRIANGLES);
    }
	shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
		angle += seconds_elapsed;
    if (keystate[SDL_SCANCODE_G])
        shader = gouraud_shader;
    if (keystate[SDL_SCANCODE_P])
        shader = phong_shader;
	if (keystate[SDL_SCANCODE_RIGHT])
		camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_LEFT])
		camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
	if (keystate[SDL_SCANCODE_UP])
		camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
	else if (keystate[SDL_SCANCODE_DOWN])
		camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
    if (keystate[SDL_SCANCODE_1])
        light2_isOn = !light2_isOn;
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
		case SDLK_r: 
			Shader::ReloadAll();
			break; //ESC key, kill the app
        
	}
}


//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
	}

}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{

	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}
