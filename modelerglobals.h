#ifndef _MODELER_GLOBALS_H
#define _MODELER_GLOBALS_H

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SampleModelControls
{ 
	XPOS, YPOS, ZPOS, HEIGHT, ROTATE, NERVOUS,
	NOSE, RIGHT_ARM, LEFT_ARM, RIGHT_LEG, LEFT_LEG, RIGHT_KNEE,LEFT_KNEE,FEET_ROTATE, LEVEL_OF_DETAILS, 
	HAND_WEAPON,TEXTURE, L_SYSTEM, L_SYSTEM_ITERATION, FRAME_ALL, INDIVIDUAL_LOOK, MOOD, 
	LIGHT_X, LIGHT_Y, LIGHT_Z, LIGHT_R, LIGHT_G, LIGHT_B, 
	LIGHT1_X, LIGHT1_Y, LIGHT1_Z, LIGHT1_R, LIGHT1_G, LIGHT1_B,
	INVERSE_KINEMATICS, CONSTRAINT_X, CONSTRAINT_Y, CONSTRAINT_Z, 
	NUMCONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f
#define COLOR_DARK_RED   1.0f,0.0f,0.0f
#define COLOR_GRAY		0.75f,0.75f,0.75f
#define COLOR_YELLOW     1.0f,1.0f,0.0f
#define COLOR_BLACK     0.0f,0.0f,0.0f
#define COLOR_WHITE     1.0f,1.0f,1.0f


// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))
#define SETVAL(control, value) (ModelerApplication::Instance()->SetControlValue(control, value))
#define ANION()( ModelerApplication::Instance()->m_animating = true) 
#define ANIOFF()( ModelerApplication::Instance()->m_animating = false) 
#endif