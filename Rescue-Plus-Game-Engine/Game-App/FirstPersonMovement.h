#pragma once
#include "Camera.h"
#include "InputManager.h"
#include "CharacterController.h"

class FirstPersonMovement :
	public UserComponent
{
private:
	InputManager* inputManager;
	CharacterController* controller;

	//Components
	Camera* camera;
	GameObject* cameraGO;

	//Control
	bool sprinting;
	bool crouching;
	bool sliding;
	bool grounded;
	bool applyGravity;
	float yRot = 0;

	//Input
	bool jump;
	short slide;
	short movementX;
	short movementZ;

	//Physics
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 slideDir;

	// --------------------------------------------------------
	// Changes for when we start a sprint
	// --------------------------------------------------------
	void StartSprint();
	// --------------------------------------------------------
	// Changes for when we end a sprint
	// --------------------------------------------------------
	void EndSprint();

	// --------------------------------------------------------
	// Changes for when we start a crouch
	// --------------------------------------------------------
	void StartCrouch();
	// --------------------------------------------------------
	// Changes for when we end a crouch
	// --------------------------------------------------------
	void EndCrouch();

	// --------------------------------------------------------
	// Changes for when we start a slide
	// --------------------------------------------------------
	void StartSlide();
	// --------------------------------------------------------
	// Changes for when we end a slide
	// --------------------------------------------------------
	void EndSlide();

	// --------------------------------------------------------
	// Calculate the camera's rotation when the player moves the mouse
	// --------------------------------------------------------
	void CalculateCameraRotFromMouse();
	
public:
	FirstPersonMovement(GameObject* gameObject);
	~FirstPersonMovement();

	void FixedUpdate(float fixedTimestep) override;
	void Update(float deltaTime) override;

	// --------------------------------------------------------
	// Factory function to safely create a First Person Movement object
	//
	// name - the name of the root game object
	// --------------------------------------------------------
	static FirstPersonMovement* CreateFirstPersonCharacter(const char* name,
		int screenWidth, int screenHeight);

	// --------------------------------------------------------
	// Get the controller's camera
	// --------------------------------------------------------
	Camera* GetCamera();
};

