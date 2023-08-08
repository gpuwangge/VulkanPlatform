#include "controller.h"
//#include <math.h>
#ifndef M_PI
#define M_PI  3.14159265f
#endif

// multiplication factors for input interaction:
// //  (these are known from previous experience)
const float ANGFACT = { M_PI / 180.f };
const float SCLFACT = { 0.005f };

// active mouse buttons (or them together):
const int LEFT = { 4 };
const int MIDDLE = { 2 };
const int RIGHT = { 1 };

// minimum allowable scale factor:
const float MINSCALE = { 0.05f };


// bool CApplication::NeedToExit = false;
// Camera CApplication::mainCamera;

bool NeedToExit;
Camera mainCamera;

CController::CController(){}
CController::~CController(){}

void CController::GLFWKeyboard(GLFWwindow * window, int key, int scancode, int action, int mods) {
	static bool				UseIndexBuffer;			// true = use both vertex and index buffer, false = just use vertex buffer
	static bool				UseLighting;			// true = use lighting for display
	static bool				UseRotate;			// true = rotate-animate, false = use mouse for interaction
	static bool				Paused;				// true means don't animate
	static int				Mode;				// 0 = use colors, 1 = use textures, ...
	static bool				Verbose;			// true = write messages into a file

	if (action == GLFW_PRESS) {
		switch (key) {
		case 262:
			mainCamera.keys.right = true;
			break;
		case 263:
			mainCamera.keys.left = true;
			break;
		case 264:
			mainCamera.keys.down = true;
			break;
		case 265:
			mainCamera.keys.up = true;
			break;
		case 'w':
		case 'W':
			mainCamera.keys.forward = true;
			break;
		case 's':
		case 'S':
			mainCamera.keys.backward = true;
			break;
		case 'a':
		case 'A':
			mainCamera.keys.turnLeft = true;
			break;
		case 'd':
		case 'D':
			mainCamera.keys.turnRight = true;
			break;

		case 'i':
		case 'I':
			UseIndexBuffer = !UseIndexBuffer;
			break;

		case 'l':
		case 'L':
			UseLighting = !UseLighting;
			break;

		case 'm':
		case 'M':
			Mode++;
			if (Mode >= 2)
				Mode = 0;
			break;

		case 'p':
		case 'P':
			Paused = !Paused;
			break;

		case 'q':
		case 'Q':
		case GLFW_KEY_ESCAPE:
			NeedToExit = true;
			break;

		case 'r':
		case 'R':
			UseRotate = !UseRotate;
			break;

		case 'v':
		case 'V':
			Verbose = !Verbose;
			break;

		default:
			//fprintf(FpDebug, "Unknown key hit: 0x%04x = '%c'\n", key, key);
			//fprintf(stderr, "Unknown key hit: 0x%04x = '%c'\n", key, key);
			//fflush(FpDebug);
			break;
		}
	}

	if (action == GLFW_RELEASE) {	
		switch (key) {
		case 262:
			mainCamera.keys.right = false;
			break;
		case 263:
			mainCamera.keys.left = false;
			break;
		case 264:
			mainCamera.keys.down = false;
			break;
		case 265:
			mainCamera.keys.up = false;
			break;
		case 'w':
		case 'W':
			mainCamera.keys.forward = false;
			break;
		case 's':
		case 'S':
			mainCamera.keys.backward = false;
			break;
		case 'a':
		case 'A':
			mainCamera.keys.turnLeft = false;
			break;
		case 'd':
		case 'D':
			mainCamera.keys.turnRight = false;
			break;
		default:
			break;
		}
	}

}

void CController::GLFWMouseMotion(GLFWwindow *window, double xpos, double ypos){
	static int				Xmouse, Ymouse;			// mouse values
	static float				Xrot, Yrot;			// rotation angles in degrees
	static int				ActiveButton;			// current button that is down
	static float				Scale;				// scaling facto

	int dx = (int)xpos - Xmouse;		// change in mouse coords
	int dy = (int)ypos - Ymouse;

	if ((ActiveButton & LEFT) != 0)
	{
		Xrot += (ANGFACT*dy);
		Yrot += (ANGFACT*dx);
	}


	if ((ActiveButton & MIDDLE) != 0)
	{
		Scale += SCLFACT * (float)(dx - dy);

		// keep object from turning inside-out or disappearing:

		if (Scale < MINSCALE)
			Scale = MINSCALE;
	}

	Xmouse = (int)xpos;			// new current position
	Ymouse = (int)ypos;
}

void CController::GLFWMouseButton(GLFWwindow *window, int button, int action, int mods) {
	static int				Xmouse, Ymouse;			// mouse values
	static int				ActiveButton;			// current button that is down

	//if (Verbose)		fprintf(FpDebug, "Mouse button = %d; Action = %d\n", button, action);

	int b = 0;		// LEFT, MIDDLE, or RIGHT


					// get the proper button bit mask:

	switch (button)
	{
	case GLFW_MOUSE_BUTTON_LEFT:
		b = LEFT;		break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		b = MIDDLE;		break;

	case GLFW_MOUSE_BUTTON_RIGHT:
		b = RIGHT;		break;

	default:
		b = 0;
		//fprintf(FpDebug, "Unknown mouse button: %d\n", button);
	}


	// button down sets the bit, up clears the bit:

	if (action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Xmouse = (int)xpos;
		Ymouse = (int)ypos;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}