/*
     File: GLEssentialsGLView.m
 Abstract: n/a
  Version: 1.7
 
 Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
 Inc. ("Apple") in consideration of your agreement to the following
 terms, and your use, installation, modification or redistribution of
 this Apple software constitutes acceptance of these terms.  If you do
 not agree with these terms, please do not use, install, modify or
 redistribute this Apple software.
 
 In consideration of your agreement to abide by the following terms, and
 subject to these terms, Apple grants you a personal, non-exclusive
 license, under Apple's copyrights in this original Apple software (the
 "Apple Software"), to use, reproduce, modify and redistribute the Apple
 Software, with or without modifications, in source and/or binary forms;
 provided that if you redistribute the Apple Software in its entirety and
 without modifications, you must retain this notice and the following
 text and disclaimers in all such redistributions of the Apple Software.
 Neither the name, trademarks, service marks or logos of Apple Inc. may
 be used to endorse or promote products derived from the Apple Software
 without specific prior written permission from Apple.  Except as
 expressly stated in this notice, no other rights or licenses, express or
 implied, are granted by Apple herein, including but not limited to any
 patent rights that may be infringed by your derivative works or by other
 works in which the Apple Software may be incorporated.
 
 The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
 MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
 THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
 OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
 
 IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
 MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
 AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
 STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 
 Copyright (C) 2013 Apple Inc. All Rights Reserved.
 
 */

#import "GLEssentialsGLView.h"
#include "GLRenderer.h"

#define SUPPORT_RETINA_RESOLUTION 1

@interface GLEssentialsGLView (PrivateMethods)
- (void) initGL;

@end

@implementation GLEssentialsGLView
{
   // LeapController *leapController;
    GLRenderer *glRenderer;
    float deltaTime;
}
//OpenGLRenderer* m_renderer;

- (CVReturn) getFrameForTime:(const CVTimeStamp*)outputTime
{
	// There is no autorelease pool when this method is called
	// because it will be called from a background thread.
	// It's important to create one or app can leak objects.
    deltaTime = 1.0 / (outputTime->rateScalar * (float)outputTime->videoTimeScale / (float)outputTime->videoRefreshPeriod);
	[self drawView];
	return kCVReturnSuccess;
}

// This is the renderer output callback function
static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
									  const CVTimeStamp* now,
									  const CVTimeStamp* outputTime,
									  CVOptionFlags flagsIn,
									  CVOptionFlags* flagsOut, 
									  void* displayLinkContext)
{
    CVReturn result = [(__bridge GLEssentialsGLView*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (void) awakeFromNib
{
    NSOpenGLPixelFormatAttribute attrs[] =
	{
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFADepthSize, 24,
		// Must specify the 3.2 Core Profile to use OpenGL 3.2
		NSOpenGLPFAOpenGLProfile,
		NSOpenGLProfileVersion3_2Core,
		0
	};
	
	NSOpenGLPixelFormat *pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	
	if (!pf)
	{
		NSLog(@"No OpenGL pixel format");
	}
	   
    NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:nil];
    
#if ESSENTIAL_GL_PRACTICES_SUPPORT_GL3 && defined(DEBUG)
	// When we're using a CoreProfile context, crash if we call a legacy OpenGL function
	// This will make it much more obvious where and when such a function call is made so
	// that we can remove such calls.
	// Without this we'd simply get GL_INVALID_OPERATION error for calling legacy functions
	// but it would be more difficult to see where that function was called.
	CGLEnable((CGLContextObj)[context CGLContextObj], kCGLCECrashOnRemovedFunctions);
#endif
	
    [self setPixelFormat:pf];
    
    [self setOpenGLContext:context];
    
#if SUPPORT_RETINA_RESOLUTION
    // Opt-In to Retina resolution
    [self setWantsBestResolutionOpenGLSurface:YES];
#endif // SUPPORT_RETINA_RESOLUTION
}

- (void) prepareOpenGL
{
	[super prepareOpenGL];
	
	// Make all the OpenGL calls to setup rendering  
	//  and build the necessary rendering objects
	[self initGL];
	
	// Create a display link capable of being used with all active displays
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	
	// Set the renderer output callback function
	CVDisplayLinkSetOutputCallback(displayLink, &MyDisplayLinkCallback, (__bridge void *)self);
	
	// Set the display link for the current renderer
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
	
	// Activate the display link
	CVDisplayLinkStart(displayLink);
	
	// Register to be notified when the window closes so we can stop the displaylink
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowWillClose:)
												 name:NSWindowWillCloseNotification
											   object:[self window]];
    
   // [self run];
}

- (void) windowWillClose:(NSNotification*)notification
{
	// Stop the display link when the window is closing because default
	// OpenGL render buffers will be destroyed.  If display link continues to
	// fire without renderbuffers, OpenGL draw calls will set errors.
	
	CVDisplayLinkStop(displayLink);
}

- (void) initGL
{
	// The reshape function may have changed the thread to which our OpenGL
	// context is attached before prepareOpenGL and initGL are called.  So call
	// makeCurrentContext to ensure that our OpenGL context current to this 
	// thread (i.e. makeCurrentContext directs all OpenGL calls on this thread
	// to [self openGLContext])
	[[self openGLContext] makeCurrentContext];
	
	// Synchronize buffer swaps with vertical refresh rate
	GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	
	// Init our renderer.  Use 0 for the defaultFBO which is appropriate for
	// OSX (but not iOS since iOS apps must create their own FBO)
    glRenderer = new GLRenderer();
    glRenderer->initOpenGL();
    [self viewDidEndLiveResize];
}

- (NSRect)getViewRect {
    // Get the view size in Points
	NSRect viewRectPoints = [self bounds];
    
#if SUPPORT_RETINA_RESOLUTION
    
    // Rendering at retina resolutions will reduce aliasing, but at the potential
    // cost of framerate and battery life due to the GPU needing to render more
    // pixels.
    
    // Any calculations the renderer does which use pixel dimentions, must be
    // in "retina" space.  [NSView convertRectToBacking] converts point sizes
    // to pixel sizes.  Thus the renderer gets the size in pixels, not points,
    // so that it can set it's viewport and perform and other pixel based
    // calculations appropriately.
    // viewRectPixels will be larger (2x) than viewRectPoints for retina displays.
    // viewRectPixels will be the same as viewRectPoints for non-retina displays
    NSRect viewRectPixels = [self convertRectToBacking:viewRectPoints];
    
#else //if !SUPPORT_RETINA_RESOLUTION
    
    // App will typically render faster and use less power rendering at
    // non-retina resolutions since the GPU needs to render less pixels.  There
    // is the cost of more aliasing, but it will be no-worse than on a Mac
    // without a retina display.
    
    // Points:Pixels is always 1:1 when not supporting retina resolutions
    NSRect viewRectPixels = viewRectPoints;
    
#endif // !SUPPORT_RETINA_RESOLUTION

    return viewRectPixels;
}

- (void) reshape
{	
	[super reshape];
	
	// We draw on a secondary thread through the display link. However, when
	// resizing the view, -drawRect is called on the main thread.
	// Add a mutex around to avoid the threads accessing the context
	// simultaneously when resizing.
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);

    NSRect viewRectPixels = [self getViewRect];
	   
	// Set the new dimensions in our renderer
    glRenderer->reshape(viewRectPixels.size.width, viewRectPixels.size.height);
	
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)renewGState
{	
	// Called whenever graphics state updated (such as window resize)
	
	// OpenGL rendering is not synchronous with other rendering on the OSX.
	// Therefore, call disableScreenUpdatesUntilFlush so the window server
	// doesn't render non-OpenGL content in the window asynchronously from
	// OpenGL content, which could cause flickering.  (non-OpenGL content
	// includes the title bar and drawing done by the app with other APIs)
	[[self window] disableScreenUpdatesUntilFlush];

	[super renewGState];
}

- (void) drawRect: (NSRect) theRect
{
	// Called during resize operations
	
	// Avoid flickering during resize by drawiing
    [self drawView];
}

- (void) drawView
{	 
	[[self openGLContext] makeCurrentContext];

	// We draw on a secondary thread through the display link
	// When resizing the view, -reshape is called automatically on the main
	// thread. Add a mutex around to avoid the threads accessing the context
	// simultaneously when resizing
	CGLLockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
    glRenderer->render(deltaTime);
    
	CGLFlushDrawable((CGLContextObj)[[self openGLContext] CGLContextObj]);
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void) dealloc
{
	// Stop the display link BEFORE releasing anything in the view
    // otherwise the display link thread may call into the view and crash
    // when it encounters something that has been release
	CVDisplayLinkStop(displayLink);
	CVDisplayLinkRelease(displayLink);
}

#pragma mark - Leap Motion Shit

//- (void)run
//{
//    leapController = [[LeapController alloc] init];
//    [leapController addListener:self];
//    NSLog(@"running");
//}
//
//- (void)onInit:(NSNotification *)notification
//{
//    NSLog(@"Initialized");
//}
//
//- (void)onConnect:(NSNotification *)notification
//{
//    NSLog(@"Connected");
////    LeapController *aController = (LeapController *)[notification object];
////    [aController enableGesture:LEAP_GESTURE_TYPE_CIRCLE enable:YES];
////    [aController enableGesture:LEAP_GESTURE_TYPE_KEY_TAP enable:YES];
////    [aController enableGesture:LEAP_GESTURE_TYPE_SCREEN_TAP enable:YES];
////    [aController enableGesture:LEAP_GESTURE_TYPE_SWIPE enable:YES];
//}
//
//- (void)onDisconnect:(NSNotification *)notification
//{
//    //Note: not dispatched when running in a debugger.
//    NSLog(@"Disconnected");
//}
//
//- (void)onServiceConnect:(NSNotification *)notification
//{
//    NSLog(@"Service Connected");
//}
//
//- (void)onServiceDisconnect:(NSNotification *)notification
//{
//    NSLog(@"Service Disconnected");
//}
//
//- (void)onDeviceChange:(NSNotification *)notification
//{
//    NSLog(@"Device Changed");
//}
//
//- (void)onExit:(NSNotification *)notification
//{
//    NSLog(@"Exited");
//}
//
//- (void)onFrame:(NSNotification *)notification
//{
//    LeapController *aController = (LeapController *)[notification object];
//    
//    // Get the most recent frame and report some basic information
//    LeapFrame *frame = [aController frame:0];
//    
////    NSLog(@"Frame id: %lld, timestamp: %lld, hands: %ld, fingers: %ld, tools: %ld, gestures: %ld",
////          [frame id], [frame timestamp], [[frame hands] count],
////          [[frame fingers] count], [[frame tools] count], [[frame gestures:nil] count]);
//    
//    for (int i = 0; i < [[frame hands] count]; i++){
//        // Get the first hand
//        LeapHand *hand = [[frame hands] objectAtIndex:i];
//        
//        // Check if the hand has any fingers
//        NSArray *fingers = [hand fingers];
//        if ([fingers count] != 0) {
//            // Calculate the hand's average finger tip position
//            LeapVector *avgPos = [[LeapVector alloc] init];
//            for (int i = 0; i < [fingers count]; i++) {
//                LeapFinger *finger = [fingers objectAtIndex:i];
//                avgPos = [avgPos plus:[finger tipPosition]];
//            }
//            avgPos = [avgPos divide:[fingers count]];
////            NSLog(@"Hand has %ld fingers, average finger tip position %@",
////                  [fingers count], avgPos);
//        }
//        
//        // Get the hand's sphere radius and palm position
////        NSLog(@"Hand sphere radius: %f mm, palm position: %@",
////              [hand sphereRadius], [hand palmPosition]);
//        
//        // Get the hand's normal vector and direction
//        const LeapVector *position = [hand palmPosition];
//        const LeapVector *velocity = [hand palmVelocity];
//        LeapVector *boxCenter = frame.interactionBox.center;
//        LeapVector *normalisedPoint = [frame.interactionBox normalizePoint:position clamp:YES];
//        
////        if (position.x > boxCenter.x){
////            glRenderer->leap_rightHandVelocity(velocity.x, velocity.y, velocity.z);
////        } else {
////            glRenderer->leap_leftHandVelocity(velocity.x, velocity.y, velocity.z);
////        }
//        
//        if (hand.isLeft){
//            glRenderer->changeParticleVelocity(velocity.x);
//        } else if (hand.isRight){
//            glRenderer->moveLightSourceByNormalisedVector(normalisedPoint.x, normalisedPoint.y, normalisedPoint.z);
//        }
//    }
//    
//        // Calculate the hand's pitch, roll, and yaw angles
////        NSLog(@"Hand pitch: %f degrees, roll: %f degrees, yaw: %f degrees\n",
////              [direction pitch] * LEAP_RAD_TO_DEG,
////              [normal roll] * LEAP_RAD_TO_DEG,
////              [direction yaw] * LEAP_RAD_TO_DEG);
//    
////    NSArray *gestures = [frame gestures:nil];
////    for (int g = 0; g < [gestures count]; g++) {
////        LeapGesture *gesture = [gestures objectAtIndex:g];
////        switch (gesture.type) {
////            case LEAP_GESTURE_TYPE_CIRCLE: {
////                LeapCircleGesture *circleGesture = (LeapCircleGesture *)gesture;
////                
////                NSString *clockwiseness;
////                if ([[[circleGesture pointable] direction] angleTo:[circleGesture normal]] <= LEAP_PI/4) {
////                    clockwiseness = @"clockwise";
////                } else {
////                    clockwiseness = @"counterclockwise";
////                }
////                
////                // Calculate the angle swept since the last frame
////                float sweptAngle = 0;
////                if(circleGesture.state != LEAP_GESTURE_STATE_START) {
////                    LeapCircleGesture *previousUpdate = (LeapCircleGesture *)[[aController frame:1] gesture:gesture.id];
////                    sweptAngle = (circleGesture.progress - previousUpdate.progress) * 2 * LEAP_PI;
////                }
////                
////                NSLog(@"Circle id: %d, %@, progress: %f, radius %f, angle: %f degrees %@",
////                      circleGesture.id, [self stringForState:gesture.state],
////                      circleGesture.progress, circleGesture.radius,
////                      sweptAngle * LEAP_RAD_TO_DEG, clockwiseness);
////                break;
////            }
////            case LEAP_GESTURE_TYPE_SWIPE: {
////                LeapSwipeGesture *swipeGesture = (LeapSwipeGesture *)gesture;
////                NSLog(@"Swipe id: %d, %@, position: %@, direction: %@, speed: %f",
////                      swipeGesture.id, [self stringForState:swipeGesture.state],
////                      swipeGesture.position, swipeGesture.direction, swipeGesture.speed);
////                break;
////            }
////            case LEAP_GESTURE_TYPE_KEY_TAP: {
////                LeapKeyTapGesture *keyTapGesture = (LeapKeyTapGesture *)gesture;
////                NSLog(@"Key Tap id: %d, %@, position: %@, direction: %@",
////                      keyTapGesture.id, [self stringForState:keyTapGesture.state],
////                      keyTapGesture.position, keyTapGesture.direction);
////                break;
////            }
////            case LEAP_GESTURE_TYPE_SCREEN_TAP: {
////                LeapScreenTapGesture *screenTapGesture = (LeapScreenTapGesture *)gesture;
////                NSLog(@"Screen Tap id: %d, %@, position: %@, direction: %@",
////                      screenTapGesture.id, [self stringForState:screenTapGesture.state],
////                      screenTapGesture.position, screenTapGesture.direction);
////                break;
////            }
////            default:
////                NSLog(@"Unknown gesture type");
////                break;
////        }
////    }
//    
////    if (([[frame hands] count] > 0) || [[frame gestures:nil] count] > 0) {
////        NSLog(@" ");
////    }
//}
//
//- (void)onFocusGained:(NSNotification *)notification
//{
//    NSLog(@"Focus Gained");
//}
//
//- (void)onFocusLost:(NSNotification *)notification
//{
//    NSLog(@"Focus Lost");
//}
//
//- (NSString *)stringForState:(LeapGestureState)state
//{
//    switch (state) {
//        case LEAP_GESTURE_STATE_INVALID:
//            return @"STATE_INVALID";
//        case LEAP_GESTURE_STATE_START:
//            return @"STATE_START";
//        case LEAP_GESTURE_STATE_UPDATE:
//            return @"STATE_UPDATED";
//        case LEAP_GESTURE_STATE_STOP:
//            return @"STATE_STOP";
//        default:
//            return @"STATE_INVALID";
//    }
//}

@end
