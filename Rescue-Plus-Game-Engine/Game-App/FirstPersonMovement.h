#pragma once
#include "Camera.h"
#include "InputManager.h"
#include "CharacterController.h"
#include "CameraShaker.h"

class FirstPersonMovement :
	public UserComponent
{
private:
	enum class SlideState{None, Starting, Sliding, Ending, EndingFromJump};
	enum class CameraState{None, Walking, Sprinting, Crouching, InAir, Sliding};

	InputManager* inputManager;
	CharacterController* controller;

	//Components
	Camera* camera;
	GameObject* cameraGO;

	//Physics
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 slideDir;

	//Camera
	CameraShaker cameraShaker;
	DirectX::XMFLOAT3 lastFrameCameraPos;
	DirectX::XMFLOAT3 cameraBasePos;
	DirectX::XMFLOAT3 cameraTargetPos;
	CameraState cameraState;
	float cameraT;
	float cameraFOVT;
	float cameraFOV;
	short cameraDir;
	bool cameraLerpToStart;

	//Input
	float xMult;
	float zMult;
	float inAirMaxSpeed;
	SlideState slideState;
	short movementX;
	short movementZ;
	bool jump;

	//Control
	bool sprinting;
	bool crouching;
	bool grounded;
	bool prevGrounded;
	bool applyGravity;

	// --------------------------------------------------------
	// Activate a camera transition
	// --------------------------------------------------------
	void CameraTransition(CameraState newState, float baseHeight, float bobMax);

	// --------------------------------------------------------
	// Apply various effects to the camera depending on the movement state
	// --------------------------------------------------------
	void ApplyHeadBob(float fixedTimestep);

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
	void EndSlide(bool fromJump);
	// --------------------------------------------------------
	// If the slide state is in a valid sliding state
	// --------------------------------------------------------
	bool IsSliding();

	// --------------------------------------------------------
	// Calculate the camera's rotation when the player moves the mouse
	// --------------------------------------------------------
	void CalculateCameraRotFromMouse(float deltaTime);
	
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

