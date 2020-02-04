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

	framebuffer.resize(w, h);
}

//Here we have already GL working, so we can create meshes and textures
void Application::init(void)
{
	std::cout << "initiating app..." << std::endl;

    for ( int i = 0; i < 100; i++)
    {
        
        particles[i].x = random() % (int)window_width;
        particles[i].y = random() % (int)window_height;
        particles[i].size = random() % (int)5;
        int randNum = random() % 255;
        particles[i].color = Color(randNum, randNum, randNum);
    }
	//here add your init stuff
    framebuffer.fill(Color::WHITE);
}

//render one frame
void Application::render( Image& framebuffer )
{
    if ( keystate[SDL_SCANCODE_SPACE] )
    {
        framebuffer.fill(Color::BLACK);
        for (unsigned int i = 0; i < 100; i++)
        {
            if (!framebuffer.snowAnimation((int)particles[i].x, (int)particles[i].y, particles[i].size, particles[i].color))
            {
                particles[i].x = random() % (int)window_width;
                particles[i].y = window_height - 1;
             }
        }
    }
    if ( render_mode == PAINT_MODE )
    {
        if( mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if ( mouse_position.x < window_width && mouse_position.x > 0 && mouse_position.y < window_height && mouse_position.y > toolBar.height)
            {
                framebuffer.drawCircle((int)mouse_position.x, (int)mouse_position.y, 2, defCol, true);
            }
        }
    }
    else
    {
        if ( render_mode == LINE_MODE)
            // framebuffer.drawLineBresenham(random() % (int)window_width, random() % (int)(window_height - toolBar.height), random() % (int)window_width, random() % (int)window_height, defCol);
            framebuffer.drawLineDDA(250, 250, 200, 500, defCol);
            framebuffer.drawLineBresenham(250, 250, 200, 500, defCol);
        if ( render_mode == CIRCLE_MODE)
            framebuffer.drawCircle(random() % (int)window_width, random() % (int)(window_height - toolBar.height), random() % (int)50, defCol, random() % 2 < 1);
        if ( render_mode == RECTANGLE_MODE)
            framebuffer.drawRectangle(random() % (int)window_width, random() % (int)(window_height - toolBar.height), random() % (int)50, random() % (int)50, defCol, random() % 2 < 1);
        if ( render_mode == CENTROID_MODE)
            framebuffer.formulaCentroid();
        if ( render_mode == GRADIENT1_MODE)
            framebuffer.gradientBlueRed();
        if ( render_mode == GRADIENT2_MODE)
            framebuffer.gradientRedGreenSteps();
        if ( render_mode == GRAYSCALE_MODE)
            framebuffer.grayScale();
        if ( render_mode == TRESHOLD_MODE)
            framebuffer.treshold();
        if ( render_mode == RESIZE_MODE )
            framebuffer.scale(2);
        if ( render_mode == ROTATE_MODE )
            framebuffer.rotate(45);
        if ( render_mode == RESTART_MODE)
            framebuffer.fill(Color::WHITE);
        render_mode = DEF_MODE;
    }
        
    
}

//called after render
void Application::update(double seconds_elapsed)
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode
	if (keystate[SDL_SCANCODE_SPACE]) //if key space is pressed
	{
		    for ( int i = 0; i < 100; i++)
            {
                int rnd = random() % 10;
                particles[i].x += (int)particles[i].y % 20 < 10 ? seconds_elapsed * rnd : seconds_elapsed * rnd * -1;
                particles[i].y -= seconds_elapsed*10;
            }
	}
	//to read mouse position use mouse_position
}

//keyboard press event 
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	//to see all the keycodes: https://wiki.libsdl.org/SDL_Keycode

	switch(event.keysym.scancode)
	{
		case SDL_SCANCODE_ESCAPE:
			exit(0); 
			break; //ESC key, kill the app
        case SDL_SCANCODE_1:
            render_mode = LINE_MODE;
            break;
        case SDL_SCANCODE_2:
            render_mode = RECTANGLE_MODE;
            break;
        case SDL_SCANCODE_3:
            render_mode = CIRCLE_MODE;
            break;
        case SDL_SCANCODE_4:
            render_mode = CENTROID_MODE;
            break;
        case SDL_SCANCODE_5:
            render_mode = GRADIENT1_MODE;
            break;
        case SDL_SCANCODE_6:
            render_mode = GRADIENT2_MODE;
            break;
        case SDL_SCANCODE_7:
            render_mode = GRAYSCALE_MODE;
            break;
        case SDL_SCANCODE_8:
            render_mode = TRESHOLD_MODE;
            break;
        case SDL_SCANCODE_9:
            render_mode = ROTATE_MODE;
            break;
        case SDL_SCANCODE_0:
            render_mode = RESIZE_MODE;
            break;
        case SDL_SCANCODE_R:
            render_mode = RESTART_MODE;
            break;
        case SDL_SCANCODE_P:
            render_mode = PARTICLE_SNOW_MODE;
            break;
        case SDL_SCANCODE_L:
            // load a placeholder image
            if (this->framebuffer.loadTGA("../../res/forest.tga") == false)
            {
                std::cout << "Image not found" << std::endl;
            }
            break;
        case SDL_SCANCODE_E:
            // load a placeholder image
            loadToolBar();
            render_mode = PAINT_MODE;
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
        int start = 11;
        int save = 60;
        // save button occupies 30px, others 25
        int saveEnd = 90;
        int colorStart = saveEnd + 25;
        if ( mouse_position.y < this->toolBar.height )
        {
            if ( mouse_position.x > start && mouse_position.x < start + 25)
            {
                loadToolBar();
            }
            if ( mouse_position.x > saveEnd - 30 && mouse_position.x < saveEnd )
            {
                Image imageToSave;
                imageToSave.resize(framebuffer.width, framebuffer.height);
                imageToSave.fill(Color::WHITE);
                for ( int x = 0; x < framebuffer.width; x++)
                {
                    for ( int y = toolBar.width; y < framebuffer.height; y++)
                    {
                        imageToSave.setPixel(x, y, framebuffer.getPixel(x, y));
                    }
                }
                if (imageToSave.saveTGA("../../res/myFile.tga") == false)
                {
                    std::cout << "Error saving the image." << std::endl;
                }
            }
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::BLACK;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::RED;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::GREEN;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::BLUE;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::YELLOW;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::PURPLE;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::CYAN;
            }
            colorStart = colorStart + 50;
            if ( mouse_position.x > colorStart && mouse_position.x < colorStart + 25 )
            {
                defCol = Color::WHITE;
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

void Application::loadToolBar()
{
    this->framebuffer.fill(Color::WHITE);
    if (this->toolBar.loadTGA("../../res/toolbar.tga") == false)
    {
        std::cout << "Image not found" << std::endl;
    }
    for ( unsigned int x = 0; x < this->toolBar.width; x++ )
    {
        for ( unsigned int y = 0; y < this->toolBar.height; y++ )
        {
            framebuffer.setPixel(x, y, this->toolBar.getPixelRef(x, y));
        }
    }
}
