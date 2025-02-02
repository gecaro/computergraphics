#include "application.h"
#include "utils.h"
#include "image.h"

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
    this->render_mode = DEF_MODE;
    this->defCol = Color::BLACK;
    
    this->points.p1.x = 0;
    this->points.p1.y = 0;
    this->points.p2.x = 0;
    this->points.p2.y = 0;
    this->points.p3.x = 0;
    this->points.p3.y = 0;
    this->points.counter = 0;
    this->fill = true;
	framebuffer.resize(w, h);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

	//here add your init stuff
    framebuffer.fill(Color::WHITE);
}

//render one frame
void Application::render( Image& framebuffer )
{
    if ( render_mode == LINE_MODE_DDA && points.counter > 1){
        framebuffer.drawLineDDA(points.p1.x, points.p1.y, points.p2.x, points.p2.y, defCol);
        points.counter = 0;
    }
    if ( render_mode == LINE_MODE_BRESENHAM && points.counter > 1){
        framebuffer.drawLineDDA(points.p1.x, points.p1.y, points.p2.x, points.p2.y, defCol);
        points.counter = 0;
    }
    if ( render_mode == CIRCLE_MODE && points.counter > 1){
        framebuffer.drawCircle(points.p1.x, points.p1.y, distance(points.p1, points.p2), defCol, fill);
        points.counter = 0;
    }
    if ( render_mode == TRIANGLE_MODE && points.counter > 2)
    {
        framebuffer.drawTriangle(points.p1.x, points.p1.y, points.p2.x, points.p2.y, points.p3.x, points.p3.y, defCol, fill);
        points.counter = 0;
    }
    if ( render_mode == TRIANGLE_INTERPOLATED_MODE && points.counter > 2)
    {
        Color red = Color::RED;
        Color green = Color::GREEN;
        Color blue = Color::BLUE;
        framebuffer.drawTriangleInterpolated(points.p1.x, points.p1.y, points.p2.x, points.p2.y, points.p3.x, points.p3.y, red, green, blue);
        points.counter = 0;
    }
    if ( render_mode == RESTART_MODE)
    {
        framebuffer.fill(Color::WHITE);
        points.counter = 0;
    }
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
    points.counter = 0;
	switch(event.keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			exit(0); 
			break; //ESC key, kill the app
        case SDL_SCANCODE_1:
            render_mode = LINE_MODE_DDA;
            break;
        case SDL_SCANCODE_2:
            render_mode = LINE_MODE_DDA;
            break;
        case SDL_SCANCODE_3:
            render_mode = CIRCLE_MODE;
            break;
        case SDL_SCANCODE_4:
            render_mode = TRIANGLE_MODE;
            break;
        case SDL_SCANCODE_5:
            render_mode = TRIANGLE_INTERPOLATED_MODE;
            break;
        case SDL_SCANCODE_F:
            fill = !fill;
            break;
        case SDL_SCANCODE_R:
            render_mode = RESTART_MODE;
            break;
	}
}

//keyboard key up event 
void Application::onKeyUp(SDL_KeyboardEvent event)
{
	//...
    //render_mode = DEF_MODE;
}

//mouse button event
void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse pressed
	{
		//if you read mouse position from the event, careful, Y is reversed, use mouse_position instead
        
	}
}

void Application::onMouseButtonUp( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_LEFT) //left mouse unpressed
	{
        if (render_mode == LINE_MODE_DDA || render_mode == CIRCLE_MODE || render_mode == LINE_MODE_BRESENHAM)
        {
            switch (points.counter) {
                case 1:
                    points.p2.x = mouse_position.x;
                    points.p2.y = mouse_position.y;
                    points.counter ++;
                    break;
                default:
                    points.p1.x = mouse_position.x;
                    points.p1.y = mouse_position.y;
                    points.counter = 1;
                    break;
            }
        }
        else if (render_mode == TRIANGLE_MODE || render_mode == TRIANGLE_INTERPOLATED_MODE)
        {
           switch (points.counter) {
               case 1:
                   points.p2.x = mouse_position.x;
                   points.p2.y = mouse_position.y;
                   points.counter ++;
                   break;
               case 2:
                   points.p3.x = mouse_position.x;
                   points.p3.y = mouse_position.y;
                   points.counter ++;
                   break;
               default:
                   points.p1.x = mouse_position.x;
                   points.p1.y = mouse_position.y;
                   points.counter = 1;
                   break;
           }
        }
	}
}

//when the app starts
void Application::start()
{
	std::cout << "launching loop..." << std::endl;
	launchLoop(this);
}

