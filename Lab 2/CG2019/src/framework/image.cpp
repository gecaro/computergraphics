#include "image.h"


Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

//copy constructor
Image::Image(const Image& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
}

//assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if(c.pixels)
	{
		pixels = new Color[width*height*sizeof(Color)];
		memcpy(pixels, c.pixels, width*height*sizeof(Color));
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}



//change image size (the old one will remain in the top-left corner)
void Image::resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = getPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

//change image size and scale the content
void Image::scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = getPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.setPixel( x, y, getPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::flipX()
{
	for(unsigned int x = 0; x < width * 0.5; ++x)
		for(unsigned int y = 0; y < height; ++y)
		{
			Color temp = getPixel(width - x - 1, y);
			setPixel( width - x - 1, y, getPixel(x,y));
			setPixel( x, y, temp );
		}
}

void Image::flipY()
{
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height * 0.5; ++y)
		{
			Color temp = getPixel(x, height - y - 1);
			setPixel( x, height - y - 1, getPixel(x,y) );
			setPixel( x, y, temp );
		}
}


//Loads an image from a TGA file
bool Image::loadTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int bytesPerPixel;
	unsigned int imageSize;

	FILE * file = fopen(filename, "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << filename << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		std::cerr << "TGA file seems to have errors or it is compressed, only uncompressed TGAs supported" << std::endl;
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	//save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	//convert to float all pixels
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			this->setPixel(x , height - y - 1, Color( tgainfo->data[pos+2], tgainfo->data[pos+1], tgainfo->data[pos]) );
		}

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::saveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	FILE *file = fopen(filename, "wb");
	if ( file == NULL )
	{
		fclose(file);
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	//tgainfo->width = header[1] * 256 + header[0];
	//tgainfo->height = header[3] * 256 + header[2];

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	//convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[(height-y-1)*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);
	return true;
}

void Image::drawRectangle(int start_x, int start_y, int w, int h, Color color, bool fill)
{
    if ( fill ){
        for (unsigned int x = start_x; x < start_x + w; x++)
        {
            for (unsigned int y = start_y; y < start_y + h; y++)
            {
                setPixel(x, y, color);
            }
        }
    }
    else
    {
        for (unsigned int x = start_x; x < start_x + w; x++)
        {
            setPixel(x, start_y, color);
            setPixel(x, start_y + h - 1, color);
        }
        for (unsigned int y = start_y; y < start_y + h; y++)
        {
            setPixel(start_x, y, color);
            setPixel(start_x + w - 1, y, color);
        }
    }
}

void Image::drawCircle( int xCenter, int yCenter, int r, Color color, bool fill)
{
    float r2, h;
    int x1square, x2square, y1square, y2square;
    r2 = r * r;
    //h = distance(xCenter, yCenter, xCenter + r, yCenter + r);
    x1square = xCenter - r;
    x2square = xCenter + r;
    y1square = yCenter - r;
    y2square = yCenter + r;
    for (unsigned int x = x1square; x < x2square; x++)
    {
        for(unsigned int y = y1square; y < y2square; y++)
        {
            if ( x < width && x > 0 && y < height && y > 0)
            {
                if( fill )
                    distance(xCenter, yCenter, x, y) < r ? setPixel(x, y, color) : (void)1;
                else
                {
                    // normalize the value for the equation, if it'sa near 1 then it'll mean its near the real border
                    float circleEq = ((x - xCenter) * (x - xCenter) + (y - yCenter) * (y - yCenter))/r2;
                    circleEq > .95 && circleEq < 1.05  ? setPixel(x, y, color) : (void)1;
                }
            }
        }
    }
}

void Image::drawLine( int x1, int y1, int x2, int y2, Color color )
{
    // if the first point is greater than the second in the x-axis, swap them
    if( x1 > x2 )
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    const int deltaX = x2 - x1;
    const int deltaY = y2 - y1;
    const float m = deltaY / (float)deltaX;
    const float b = y1 - m * x1;
    int y = y1;
    for(unsigned int x = x1; x < x2; x++)
    {
        y = round(m * x + b);
        setPixel(x, y, color);
    }
}

void Image::formulaCentroid()
{
    // calculate the center of the image
    const int xCenter = width / 2;
    const int yCenter = height / 2;
    // max distance to normalize it's going to be the one from 0,0 to the center
    const float maxDist = distance(0, 0, xCenter, yCenter);
    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            float normDist = distance(x, y, xCenter, yCenter) / maxDist;
            
            setPixel(x, y, Color( 255 * normDist, 255 * normDist, 255 * normDist ));
        }
    }
}

void Image::gradientBlueRed()
{
    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            // more distance in the x axis means more blue, less distance, more red
            float xDist = x / (float)width;
            setPixel(x, y, Color(255 * xDist, 0, 255 * ( 1 - xDist)));
        }
    }
}

void Image::gradientRedGreenSteps()
{
    float red = 0.0;
    float green = 0.0;
    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            // more distance in the x axis means more blue, less distance, more red
            float xDist = x / (float)width;
            float yDist = y / (float)height;
            if (x % 80 == 0)
            {
                red = xDist * 255;
            }
            if (y % 60 == 0)
            {
                green = yDist * 255;
            }
            setPixel(x, y, Color(red, green, 0));
        }
    }
}

void Image::grayScale()
{
    Color pixel;
    float meanValue;
    for (unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++ )
        {
            pixel = getPixel(x, y);
            meanValue = (pixel.r + pixel.g + pixel.b) / 3;
            setPixel(x, y, Color(meanValue, meanValue, meanValue));
        }
    }
}

void Image::treshold()
{
    float colorVal;
    grayScale();
    for ( unsigned int x = 0; x < width; x++)
    {
        for ( unsigned y = 0; y < height; y++)
        {
            getPixel(x, y).b < 128 ? colorVal = 0 : colorVal = 255;
            setPixel(x, y, Color(colorVal, colorVal, colorVal));
        }
    }
}

void Image::rotate( int angle )
{
    //int xNew, yNew;
    Color* new_pixels = new Color[width*height];
    float sin45 = sin( angle * 3.14159 / 180 );
    float cos45 = cos( angle * 3.14159 / 180 );
    int xCenter = (int)(.5 * (width - 1));
    int yCenter = (int)(.5 * (height - 1));
    
    for ( unsigned int x = 0; x < width; x++)
    {
        for (unsigned int y = 0; y < height; y++)
        {
            int a = x - xCenter;
            int b = y - yCenter;
            int xNew = (int)(a * cos45 - b * sin45 + xCenter);
            int yNew = (int)(a * sin45 + b * cos45 + yCenter);
            if (xNew < this->width && yNew < height && xNew >=0 && yNew >= 0)
                new_pixels[y * width + x] = getPixel(xNew, yNew);
        }
    }
    delete pixels;
    pixels = new_pixels;
}

void Image::scale( float proportion )
{
    Color* new_pixels = new Color[width*height];
    for ( unsigned int x = 0; x < width; x++)
    {
        for ( unsigned int y = 0; y < height; y++)
        {
            int xNew = x / proportion;
            int yNew = y / proportion;
            if (xNew < this->width && yNew < height && xNew >=0 && yNew >= 0)
                new_pixels[y * width + x] = getPixel(xNew, yNew);
        }
    }
    delete pixels;
    pixels = new_pixels;
}

bool Image::snowAnimation(int x, int y, int size, Color color)
{
    if( x > 0 && x < width && y > 0 && y < height)
    {
        drawCircle(x, y, size, color, true);
        return true;
    }
    else return false;
}

void Image::drawLineDDA(int x0, int y0, int x1, int y1, Color & c)
{
    float d, x, y, vx, vy;
    float dx = x1 - x0;
    float dy = y1 - y0;
    d = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);
    vx = dx / d;
    vy = dy / d;
    x = x0;
    y = y0;
    for ( int i = 0; i < d; i++)
    {
        setPixel(floor(x), floor(y), c);
        x += vx;
        y += vy;
    }
}

void Image::drawLineBresenham(int x0, int y0, int x1, int y1, Color & c)
{
    int dx, dy, incE, incNE, d , x, y;
    // check if point 0 is greater
    if( x0 > x1 )
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int sign = y0 < y1 ? 1 : -1;
    dx = x1 - x0;
    dy = (y1 - y0) * sign;
    // check if we are on the 2nd, 3rd, 6th or 7th octant
    if (dx >= dy)
    {
        d = 2 * dy - dx;
        incE = 2 * dy;
    }
    else
    {
        d = 2 * dx - dy;
        incE = 2 * dx;
    }
    incNE = 2 * (dy - dx);
    x = x0;
    y = y0;
    setPixel(x, y, c);
    while ( true )
    {
        
        if (d <= 0)
        {
            if (dx >= dy)
                x += 1;
            else
                y += 1 * sign;
            d += incE;
        }
        else
        {
            x += 1;
            y += 1 * sign;
            d += incNE;
        }
        setPixel(x, y, c);
        if (dx >= dy)
        {
            if(x == x1)
                break;
        }
        else
        {
            if (y == y1)
                break;
        }
            
    }
    
}


#ifndef IGNORE_LAMBDAS

//you can apply and algorithm for two images and store the result in the first one
//forEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void forEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif
