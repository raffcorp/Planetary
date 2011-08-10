//
//  Stats.cpp
//  Kepler
//
//  Created by Tom Carden on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Stats.h"
#include <sstream>
#include "cinder/gl/gl.h"
#include "cinder/Text.h"

using namespace std;
using namespace ci;

void Stats::setup(const Font &font, const Color &fpsColor, const Color &color)
{
    mFont = font;
    mFpsColor = fpsColor;
    mColor = color;
}

void Stats::update(const float &fps, 
                   const float &playheadTime, 
                   const float &fov, 
                   const float &currentLevel, 
                   const float &zoom)
{
    stringstream s;
	TextLayout layout;	
	layout.setFont( mFont );
	layout.setColor( mColor );

	s.str("");
	s << "FPS: " << fps;
	layout.addLine( s.str() );
	
	mParamsTex = gl::Texture( layout.render( true, false ) );    
}

void Stats::draw(const Matrix44f &mtx)
{
    if (mParamsTex) {
        glPushMatrix();
        glMultMatrixf( mtx );
        gl::color( Color::white() );
        gl::draw( mParamsTex, Vec2f( 23.0f, 25.0f ) );
        glPopMatrix();    
    }
}
