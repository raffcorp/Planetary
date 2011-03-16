/*
 *  PlayControls.h
 *  Kepler
 *
 *  Created by Tom Carden on 3/7/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/app/TouchEvent.h"
#include "cinder/Text.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#include <sstream>

template <class T>
inline std::string to_string( const T& t )
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

class PlayControls {
public:

	enum PlayButton { NO_BUTTON, PLAY_PAUSE, NEXT_TRACK, PREVIOUS_TRACK, SLIDER, ACCEL, DBUG };
	enum ButtonTexId { TEX_PLAY, TEX_PAUSE, TEX_PREV, TEX_NEXT, TEX_ACCEL_OFF, TEX_ACCEL_ON, TEX_DEBUG_OFF, TEX_DEBUG_ON };	
	
	void setup( AppCocoaTouch *app, bool initialPlayState )
	{
		mApp			= app;
		cbTouchesBegan	= mApp->registerTouchesBegan( this, &PlayControls::touchesBegan );
		cbTouchesEnded	= 0;
		cbTouchesMoved	= 0;		
		lastTouchedType = NO_BUTTON;
		prevDrawY		= 0;
		mIsPlaying		= initialPlayState;
		mBlueGlow		= Color( 20/255.0f, 163/255.0f, 240/255.0f );
		mMinutes		= 0;
		mSeconds		= 60;
		mPrevSeconds	= 0;
	}
	
	void update()
	{
		// TODO: update anything time based here, e.g. elapsed time of track playing
		// or e.g. button animation

		// clean up listeners here, because if we remove in touchesEnded then things get crazy
		if (mApp->getActiveTouches().size() == 0 && cbTouchesEnded != 0) {
			mApp->unregisterTouchesEnded( cbTouchesEnded );
			mApp->unregisterTouchesMoved( cbTouchesMoved );
			cbTouchesEnded = 0;
			cbTouchesMoved = 0;
		}		
	}

	bool touchesBegan( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		if (touches.size() > 0 && touches[0].getY() > prevDrawY) {
			if (cbTouchesEnded == 0) {
				cbTouchesEnded = mApp->registerTouchesEnded( this, &PlayControls::touchesEnded );
				cbTouchesMoved = mApp->registerTouchesMoved( this, &PlayControls::touchesMoved );			
			}
			lastTouchedType = findButtonUnderTouches(touches);
			return true;
		}
		else {
			lastTouchedType = NO_BUTTON;
		}
		return false;
	}

	bool touchesMoved( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		lastTouchedType = findButtonUnderTouches(touches);
		return false;
	}	
	
	bool touchesEnded( TouchEvent event )
	{
		vector<TouchEvent::Touch> touches = event.getTouches();
		if (lastTouchedType != NO_BUTTON && lastTouchedType == findButtonUnderTouches(touches)) {
			mCallbacksButtonPressed.call(lastTouchedType);
		}
		lastTouchedType = NO_BUTTON;
		return false;
	}
	
	void setPlaying(bool playing) {
		mIsPlaying = playing;
	}
	
	bool isPlaying()
	{
		return mIsPlaying;
	}


	void draw( const vector<gl::Texture> &texs, const gl::Texture &sliderBgTex, const Font &font, float y, float currentTime, float totalTime )
	{
		prevDrawY = y;
		
		gl::color( Color( 0.0f, 0.0f, 0.0f ) );
		gl::drawSolidRect( Rectf(0, y, getWindowWidth(), y + 45.0f ) ); // TODO: make height settable in setup()?

		touchRects.clear();
		touchTypes.clear(); // technically touch types never changes, but whatever
		
		float bWidth = 50.0f;
		float bHeight = 40.0f;
	

		// TODO: make these members?
		float x = getWindowWidth() * 0.5f - bWidth * 1.5f;
		float y1 = y + 5;
		float y2 = y1 + bHeight;
		Rectf prevButton( x,				 y1, x + bWidth,		y2 );
		Rectf playButton( x + bWidth,		 y1, x + bWidth * 2.0f, y2 );
		Rectf nextButton( x + bWidth * 2.0f, y1, x + bWidth * 3.0f, y2 );
		Rectf accelButton( getWindowWidth() - 65.0f, y1, getWindowWidth() - 15.0f, y2 );
		Rectf debugButton( 15.0f, y1, 65.0f, y2 );
		
		float sliderWidth	= sliderBgTex.getWidth();
		float sliderHeight	= sliderBgTex.getHeight();
		float sliderInset	= ( getWindowWidth() - sliderWidth ) * 0.5f;
		float playheadPer	= 0.0f;
		if( totalTime > 0.0f ){
			 playheadPer = currentTime/totalTime;
		}
		float playheadX		= ( sliderWidth - 12 ) * playheadPer;
		float bgx1			= sliderInset;
		float bgx2			= bgx1 + sliderWidth;
		float bgy1			= y + 75.0f - sliderHeight - 10;
		float bgy2			= bgy1 + sliderHeight;
		float fgx1			= bgx1 + 7;
		float fgx2			= fgx1 + playheadX;
		float fgy1			= bgy1 + 7;
		float fgy2			= bgy2 - 7;
		
		Rectf playheadSliderBg(  bgx1, bgy1, bgx2, bgy2 );
		Rectf playheadSliderBar( fgx1, fgy1, fgx2, fgy2 );
						
		touchRects.push_back( prevButton );
		touchTypes.push_back( PREVIOUS_TRACK );
		touchRects.push_back( playButton );
		touchTypes.push_back( PLAY_PAUSE );
		touchRects.push_back( nextButton );
		touchTypes.push_back( NEXT_TRACK );
		touchRects.push_back( accelButton );
		touchTypes.push_back( ACCEL );
		touchRects.push_back( debugButton );
		touchTypes.push_back( DBUG );
		Color blue( 0.2f, 0.2f, 0.5f );
		
// PREV
		gl::color( Color::white() );
		texs[ TEX_PREV ].enableAndBind();
		gl::drawSolidRect( prevButton );
		
		
// PLAY/PAUSE		
		if (mIsPlaying) texs[ TEX_PAUSE ].enableAndBind();
		else			texs[ TEX_PLAY ].enableAndBind();
		gl::drawSolidRect( playButton );
		
		
// NEXT		
		texs[ TEX_NEXT ].enableAndBind();
		gl::drawSolidRect( nextButton );
		
		
// ACCEL		
		if( G_ACCEL )	texs[ TEX_ACCEL_ON ].enableAndBind();
		else			texs[ TEX_ACCEL_OFF ].enableAndBind();
		gl::drawSolidRect( accelButton );

// DBUG		
		if( G_DEBUG )	texs[ TEX_DEBUG_ON ].enableAndBind();
		else			texs[ TEX_DEBUG_OFF ].enableAndBind();
		gl::drawSolidRect( debugButton );
        
// SLIDER BG		
		sliderBgTex.enableAndBind();
		gl::drawSolidRect( playheadSliderBg );

// CURRENT TIME
		mMinutes		= floor( currentTime/60 );
		mPrevSeconds	= mSeconds;
		mSeconds		= (int)currentTime%60;
		
		mMinutesTotal	= floor( totalTime/60 );
		mSecondsTotal	= (int)totalTime%60;
		
		double timeLeft = totalTime - currentTime;
		mMinutesLeft	= floor( timeLeft/60 );
		mSecondsLeft	= (int)timeLeft%60;
		
		if( mSeconds != mPrevSeconds ){
			string minsStr = to_string( mMinutes );
			string secsStr = to_string( mSeconds );
			if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
			if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
			stringstream ss;
			ss << minsStr << ":" << secsStr;
			TextLayout layout;
			layout.setFont( font );
			layout.setColor( ColorA( mBlueGlow, 0.5f ) );
			layout.addLine( ss.str() );
			mCurrentTimeTex = layout.render( true, false );
			
			
			
			minsStr = to_string( mMinutesLeft );
			secsStr = to_string( mSecondsLeft );
			if( minsStr.length() == 1 ) minsStr = "0" + minsStr;
			if( secsStr.length() == 1 ) secsStr = "0" + secsStr;		
			
			ss.str("");
			ss << "-" << minsStr << ":" << secsStr;
			TextLayout layout2;
			layout2.setFont( font );
			layout2.setColor( ColorA( mBlueGlow, 0.5f ) );
			layout2.addLine( ss.str() );
			mRemainingTimeTex = layout2.render( true, false );
		}
		gl::draw( mCurrentTimeTex,		Vec2f( 28.0f, bgy1-1 ) );
		gl::draw( mRemainingTimeTex,	Vec2f( bgx2 + 7.0f, bgy1-1 ) );
		
		
// SLIDER PER
		glDisable( GL_TEXTURE_2D );
		gl::color( Color( mBlueGlow * 0.25f ) );
		gl::drawSolidRect( Rectf( playheadSliderBar.x1-1, playheadSliderBar.y1-1, playheadSliderBar.x2+1, playheadSliderBar.y2+1 ) );
		
		gl::color( Color( mBlueGlow * 0.5) );
		gl::drawSolidRect( playheadSliderBar );
		
		gl::color( Color( mBlueGlow ) );
		gl::drawSolidRect( Rectf( playheadSliderBar.x1+1, playheadSliderBar.y1+1, playheadSliderBar.x2-1, playheadSliderBar.y2-1 ) );
		
		
		
	}
	
	// !!! EVENT STUFF (slightly nicer interface for adding listeners)
	template<typename T>
	CallbackId registerButtonPressed( T *obj, bool (T::*callback)(PlayButton) )
	{
		return mCallbacksButtonPressed.registerCb(std::bind1st(std::mem_fun(callback), obj));
	}	
	
private:
					  
	AppCocoaTouch *mApp;
	
	// updated by draw() so that we can test in touchesEnded
	vector<Rectf> touchRects;
	vector<PlayButton> touchTypes;
	PlayButton lastTouchedType;
	float prevDrawY;
	bool mIsPlaying;
	Color mBlueGlow;
	int mMinutes, mMinutesTotal, mMinutesLeft;
	int mSeconds, mSecondsTotal, mSecondsLeft;
	int mPrevSeconds;
	gl::Texture mCurrentTimeTex;
	gl::Texture mRemainingTimeTex;
	
	CallbackId cbTouchesBegan;
	CallbackId cbTouchesMoved;
	CallbackId cbTouchesEnded;
			
	PlayButton findButtonUnderTouches(vector<TouchEvent::Touch> touches) {
		for (int j = 0; j < touches.size(); j++) {
			TouchEvent::Touch touch = touches[j];
			if (touch.getY() < prevDrawY) {
				continue;
			}
			for (int i = 0; i < touchRects.size(); i++) {
				Rectf rect = touchRects[i];
				if (rect.contains(touch.getPos())) {
					return touchTypes[i];
				}
			}		
		}		
		return NO_BUTTON;
	}
	
	// !!! EVENT STUFF (keep track of listeners)
	CallbackMgr<bool(PlayButton)> mCallbacksButtonPressed;
	
};