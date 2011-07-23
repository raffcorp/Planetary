//
//  UIOrientationNode.h
//  Kepler
//
//  Created by Tom Carden on 7/23/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#pragma once
#include <map>
#include "cinder/Function.h"
#include "UINode.h"
#include "Orientation.h" // can't forward declare enums

// forward declare to avoid full include
namespace cinder { namespace app {
    class OrientationHelper;
    class OrientationEvent;
} }

class UIOrientationNode;
typedef std::shared_ptr<UIOrientationNode> UIOrientationNodeRef;

class UIOrientationNode : public UINode {
    
public:
    
    UIOrientationNode( ci::app::OrientationHelper *orientationHelper );
    virtual ~UIOrientationNode();
    
    ci::app::Orientation getInterfaceOrientation() { return mInterfaceOrientation; }
    void setInterfaceOrientation( const ci::app::Orientation &orientation, bool animate );
    
    void enableAnimation( bool enable = true ) { mEnableAnimation = enable; }
    bool isAnimationEnabled() { return mEnableAnimation; }
    
    // useful for syncing 3D world to 2D UI animation
    float getInterfaceAngle() { return mInterfaceAngle; }
    
    // animate mRoot interfaceSize and this node's transform matrix
    virtual void update();
        
protected:
    
	bool orientationChanged( ci::app::OrientationEvent event );
    
    float getOrientationAngle( const ci::app::Orientation &orientation );
    
    ci::app::OrientationHelper *mOrientationHelper;
    
    ci::CallbackId cbOrientationChanged;
    
    bool mEnableAnimation;
    bool mCurrentlyAnimating;
    
    ci::app::Orientation mInterfaceOrientation;
    float                mInterfaceAngle;       // animated, not always right-angle   
    ci::Vec2f            mTargetInterfaceSize;  // depends on mInterfaceOrientation
    float                mTargetInterfaceAngle; // normalized for shortest rotation animation
    
    // for lerping:
    float mLastOrientationChangeTime;
    float mOrientationAnimationDuration;
    float mPrevInterfaceAngle;
    ci::Vec2f mPrevInterfaceSize;
};