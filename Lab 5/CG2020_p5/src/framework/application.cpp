#include "application.h"
#include "utils.h"
#include "image.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "light.h"

Camera* camera = NULL;
Mesh* mesh = NULL;
Matrix44 model_matrix;
Shader* shader = NULL;
Texture* texture = NULL;
Texture* normal = NULL;
Light* light = NULL;

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
	mesh->loadOBJ("lee.obj");

	//load the texture
	texture = new Texture();
	if(!texture->load("lee_color_specular.tga"))
	{
		std::cout << "Texture not found" << std::endl;
		exit(1);
	}
    normal = new Texture();
    if(!normal->load("lee_normal.tga"))
    {
        std::cout << "Texture not found" << std::endl;
        exit(1);
    }

	//we load a shader
	shader = Shader::Get("texture.vs","texture.ps");
    light = new Light();
	//load whatever you need here
	//......
}

//render one frame
void Application::render(void)
{
	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable( GL_DEPTH_TEST );

	//Get the viewprojection
	camera->aspect = window_width / window_height;
	Matrix44 viewprojection = camera->getViewProjectionMatrix();
	
	//enable the shader
	shader->enable();
	shader->setMatrix44("model", model_matrix); //upload info to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload info to the shader
    shader->setVector3("lightpos", light->position);
    shader->setVector3("eyepos", camera->eye);
    shader->setVector3("ia", Vector3(.23, .23, .23));
    shader->setVector3("id", light->diffuse_color);
    shader->setVector3("is", light->specular_color);

	shader->setTexture("color_texture", texture, 0 ); //set texture in slot 0
    shader->setTexture("normal_texture", normal, 1);

	//render the data
	mesh->render(GL_TRIANGLES);

	//disable shader
	shader->disable();

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

//called after render
void Application::update(double seconds_elapsed)
{
	if (keystate[SDL_SCANCODE_SPACE])
	{
		model_matrix.rotateLocal(seconds_elapsed,Vector3(0,1,0));
	}
    if (keystate[SDL_SCANCODE_RIGHT])
        camera->eye = camera->eye + Vector3(1, 0, 0) * seconds_elapsed * 10.0;
    else if (keystate[SDL_SCANCODE_LEFT])
        camera->eye = camera->eye + Vector3(-1, 0, 0) * seconds_elapsed * 10.0;
    if (keystate[SDL_SCANCODE_UP])
        camera->eye = camera->eye + Vector3(0, 1, 0) * seconds_elapsed * 10.0;
    else if (keystate[SDL_SCANCODE_DOWN])
        camera->eye = camera->eye + Vector3(0, -1, 0) * seconds_elapsed * 10.0;
}

//keyboard press event 
void Application::onKeyPressed( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_r: Shader::ReloadAll(); break;
		case SDLK_ESCAPE: exit(0); break; //ESC key, kill the app
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
