/*** image.h  Javi Agenjo (javi.agenjo@gmail.com) 2013
	This file defines the class Image that allows to manipulate images.
	It defines all the need operators for Color and Image.
	It has a TGA loader and saver.
***/

#ifndef IMAGE_H
#define IMAGE_H

#include <string.h>
#include <stdio.h>
#include <iostream>
#include "framework.h"

//remove unsafe warnings
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

//Class Image: to store a matrix of pixels
class Image
{
	//a general struct to store all the information about a TGA file
	typedef struct sTGAInfo 
	{
		unsigned int width;
		unsigned int height;
		unsigned int bpp; //bits per pixel
		unsigned char* data; //bytes with the pixel information
	} TGAInfo;
    
    typedef struct sCelda
    {
        int maxX;
        int minX;
    } Celda;


public:
	unsigned int width;
	unsigned int height;
	Color* pixels;

	// CONSTRUCTORS 
	Image();
	Image(unsigned int width, unsigned int height);
	Image(const Image& c);
	Image& operator = (const Image& c); //assign operator

	//destructor
	~Image();

	//get the pixel at position x,y
	Color getPixel(unsigned int x, unsigned int y) const { return pixels[ y * width + x ]; }
	Color& getPixelRef(unsigned int x, unsigned int y)	{ return pixels[ y * width + x ]; }
	Color getPixelSafe(unsigned int x, unsigned int y) const {	
		x = clamp((unsigned int)x, 0, width-1); 
		y = clamp((unsigned int)y, 0, height-1); 
		return pixels[ y * width + x ]; 
	}

	//set the pixel at position x,y with value C
	inline void setPixel(unsigned int x, unsigned int y, const Color& c) { pixels[ y * width + x ] = c; }
	inline void setPixelSafe(unsigned int x, unsigned int y, const Color& c) const { x = clamp(x, 0, width-1); y = clamp(y, 0, height-1); pixels[ y * width + x ] = c; }

	void resize(unsigned int width, unsigned int height);
	void scale(unsigned int width, unsigned int height);
	
	void flipY(); //flip the image top-down
	void flipX(); //flip the image left-right

	//fill the image with the color C
	void fill(const Color& c) { for(unsigned int pos = 0; pos < width*height; ++pos) pixels[pos] = c; }

	//returns a new image with the area from (startx,starty) of size width,height
	Image getArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height);

	//save or load images from the hard drive
	bool loadTGA(const char* filename);
	bool saveTGA(const char* filename);
    
    // primitives (LAB 2)
    void drawLineDDA(int x0, int y0, int x1, int y1, Color & c);
    void drawLineBresenham(int x0, int y0, int x1, int y1, Color & c);
    void setPixelWithCheck(int x, int y, Color &c);
    void drawCircleAux(int x, int y, int x0, int y0, Color &c, bool fill);
    void drawCircle(int x, int y, int r, Color & c, bool fill);
    void DDAwithTable(int x0, int y0, int x1, int y1, std::vector<sCelda> &table);
    void drawTriangle(int x0, int y0, int x1, int y1, int x2, int y2, Color & c, bool fill);
    void drawTriangleInterpolated(int x0, int y0, int x1, int y1, int x2, int y2, Color & c0, Color & c1, Color & c2);
    
    
    
    void drawRectangle( int start_x, int start_y, int w, int h, Color color, bool fill);
    void drawLine( int x1, int y1, int x2, int y2, Color color );
    
    void formulaCentroid();
    void gradientBlueRed();
    void gradientRedGreenSteps();
    
    void grayScale();
    void treshold();
    
    void rotate( int angle );
    void scale( float proportion);
    
    bool snowAnimation( int x, int y, int size, Color color  );
	//used to easy code
	#ifndef IGNORE_LAMBDAS
	//applies an algorithm to every pixel in an image
	// you can use lambda sintax:   img.forEachPixel( [](Color c) { return c*2; });
	// or callback sintax:   img.forEachPixel( mycallback ); //the callback has to be Color mycallback(Color c) { ... }
	template <typename F>
	Image& forEachPixel( F callback )
	{
		for(unsigned int pos = 0; pos < width*height; ++pos)
			pixels[pos] = callback(pixels[pos]);
		return *this;
	}

	#endif


};

#endif
