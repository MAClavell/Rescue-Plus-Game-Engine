#pragma once
#include "Windows.h"
#include <DirectXMath.h>
#include <map>
#include <array>
#include "InputCodes.h"

// Singleton basis from: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern

// --------------------------------------------------------
// Singleton
//
// Handles input from keyboard and mouse
// --------------------------------------------------------
class InputManager
{
private:
	//Keyboard Control
	std::array<SHORT, 256> keyboardState;
	std::array<SHORT, 256> prevKeyboardState;

	//Mouse control
	POINT mousePos;
	POINT prevMousePos;
	bool mb_L_Down;
	bool mb_R_Down;
	bool mb_M_Down;
	bool prev_MB_L_Down;
	bool prev_MB_R_Down;
	bool prev_MB_M_Down;

	//Scrollwheel control
	float wheelDelta;

	//Window control
	HWND hWnd;
	bool winRequireFocus;
	bool windowFocused;

	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the input manager
	// --------------------------------------------------------
	InputManager() { }

	// --------------------------------------------------------
	// Destructor for when the singleton instance is deleted
	// --------------------------------------------------------
	~InputManager() { };

public:
	// --------------------------------------------------------
	// Initialize default values
	// (CALL ONLY ONCE AT THE VERY START OF THE GAME)
	// --------------------------------------------------------
	void Init(HWND hWnd);

	// --------------------------------------------------------
	// Deinitialize values
	// --------------------------------------------------------
	void Release();

	// --------------------------------------------------------
	// Get the singleton instance of the renderer
	// --------------------------------------------------------
	static InputManager* GetInstance()
	{
		static InputManager instance;

		return &instance;
	}

	//Delete these functions
	InputManager(InputManager const&) = delete;
	void operator=(InputManager const&) = delete;

	// --------------------------------------------------------
	// Change the window focus requirement
	//
	// windowMustBeFocused - Whether the window must be focused
	//						 for input to be captured
	// --------------------------------------------------------
	void SetWindowFocusRequirement(bool windowMustBeFocused);
	
	// --------------------------------------------------------
	// Get the focus of the window
	// If the focus requirement is false, then this will always return true
	// --------------------------------------------------------
	bool IsWindowFocused();


	// --------------------------------------------------------
	// Caputure the mouse so we keep getting mouse move events even if the
	// mouse leaves the window
	// --------------------------------------------------------
	void CaptureWindow();

	// --------------------------------------------------------
	// Release the control of the mouse
	// --------------------------------------------------------
	void ReleaseWindow();

	// --------------------------------------------------------
	// NOT FOR USE OUTSIDE OF GAME.CPP
	// Helper method for mouse clicking.
	// --------------------------------------------------------
	void OnMouseDown(WPARAM buttonState, int x, int y);

	// --------------------------------------------------------
	// NOT FOR USE OUTSIDE OF GAME.CPP
	// Helper method for mouse release
	// --------------------------------------------------------
	void OnMouseUp(WPARAM buttonState, int x, int y, int button);

	// --------------------------------------------------------
	// NOT FOR USE OUTSIDE OF GAME.CPP
	// Helper method for mouse wheel scrolling.  
	// WheelDelta may be positive or negative, depending 
	// on the direction of the scroll
	// --------------------------------------------------------
	void OnMouseWheel(float wheelDelta, int x, int y);

	// --------------------------------------------------------
	//Update the focus state of the window
	// --------------------------------------------------------
	void InputManager::UpdateFocus();

	// --------------------------------------------------------
	// Update the input manager's key/button states (only call ONCE PER FRAME!)
	// --------------------------------------------------------
	void UpdateStates();

	// --------------------------------------------------------
	// Update the mouse position (only call ONCE PER FRAME!)
	// --------------------------------------------------------
	void UpdateMousePos();

	// --------------------------------------------------------
	// Returns true while the inputted key is held down
	// --------------------------------------------------------
	bool GetKey(Key key);

	// --------------------------------------------------------
	// Returns true during the frame the user pressed down the inputted key
	// --------------------------------------------------------
	bool GetKeyDown(Key key);

	// --------------------------------------------------------
	// Returns true the first frame the user releases the inputted key
	// --------------------------------------------------------
	bool GetKeyUp(Key key);

	// --------------------------------------------------------
	// Returns true while the inputted mouse button is held down
	// --------------------------------------------------------
	bool GetMouseButton(MouseButtons button);

	// --------------------------------------------------------
	// Returns true during the frame the user pressed down the mouse button
	// --------------------------------------------------------
	bool GetMouseButtonDown(MouseButtons button);

	// --------------------------------------------------------
	//Returns true the first frame the user releases the mouse button
	// --------------------------------------------------------
	bool GetMouseButtonUp(MouseButtons button);

	// --------------------------------------------------------
	// If the mouse moved between this frame and the last
	// --------------------------------------------------------
	bool DidMouseMove();

	// --------------------------------------------------------
	// Get the current X mouse position
	// --------------------------------------------------------
	long GetMouseX();
	// --------------------------------------------------------
	// Get the current Y mouse position
	// --------------------------------------------------------
	long GetMouseY();

	// --------------------------------------------------------
	// Get the previous frame's X mouse position
	// --------------------------------------------------------
	long GetPreviousMouseX();
	// --------------------------------------------------------
	// Get the previous frame's Y mouse position
	// --------------------------------------------------------
	long GetPreviousMouseY();

	// --------------------------------------------------------
	// Get the current scrollwheel delta
	// --------------------------------------------------------
	float GetScrollWheelDelta();

	// --------------------------------------------------------
	// Get the X coordinate of the window (left)
	// --------------------------------------------------------
	long GetWindowX();

	// --------------------------------------------------------
	// Get the Y coordinate of the window (top)
	// --------------------------------------------------------
	long GetWindowY();

	// --------------------------------------------------------
	// Get the center X coordinate of the window
	// --------------------------------------------------------
	long GetWindowCenterX();

	// --------------------------------------------------------
	// Get the center Y coordinate of the window
	// --------------------------------------------------------
	long GetWindowCenterY();

	// --------------------------------------------------------
	// Get window width
	// --------------------------------------------------------
	long GetWindowWidth();

	// --------------------------------------------------------
	// Get window height
	// --------------------------------------------------------
	long GetWindowHeight();
};
