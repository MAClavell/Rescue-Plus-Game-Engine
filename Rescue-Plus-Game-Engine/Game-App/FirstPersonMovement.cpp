#include "FirstPersonMovement.h"
#include "ExtendedMath.h"
#include "PhysicsManager.h"
#include "Raycast.h"

using namespace DirectX;

#define MOVE_SPEED 10
#define SPRINT_SPEED 14
#define CROUCH_SPEED 5
#define JUMP_HORIZONTAL_SPEED 5
#define ACC_MULT 6
#define ACC_JUMP_MULT 2
#define SLIDE_SPEED (SPRINT_SPEED * 100)
#define SLIDE_FRICTION (SLIDE_SPEED * 0.01f)
#define SLIDE_CUTOFF 1.0f
#define JUMP_SPEED 500
#define STAND_HEIGHT 2.0f
#define CROUCH_HEIGHT 1.5f
#define SLIDE_HEIGHT 1.0f
#define SLIDE_BODY_LENGTH 3.0f

FirstPersonMovement::FirstPersonMovement(GameObject* gameObject) : UserComponent(gameObject)
{
	inputManager = InputManager::GetInstance();

	controller = gameObject->GetComponent<CharacterController>();

	auto children = gameObject->GetChildren();

	//Get Camera
	camera = children[0]->GetComponent<Camera>();
	cameraGO = children[0];

	sprinting = false;
	grounded = false;
	prevGrounded = false;
	crouching = false;
	applyGravity = true;

	jump = false;
	slideState = SlideState::None;
	movementX = 0;
	movementZ = 0;
	xMult = 0;
	zMult = 0;
	xInAirMult = 0;
	zInAirMult = 0;

	velocity = XMFLOAT3(0, 0, 0);
	slideDir = XMFLOAT3(0, 0, 0);
}
FirstPersonMovement::~FirstPersonMovement()
{ }

// Factory function to safely create a First Person Movement object
FirstPersonMovement* FirstPersonMovement::CreateFirstPersonCharacter(const char* name,
	int screenWidth, int screenHeight)
{
	//Root object
	GameObject* root = new GameObject(name);
	root->AddComponent<CharacterController>(1, STAND_HEIGHT)->SetCollisionLayerType(CollisionLayer::Player);

	//Camera object
	GameObject* camera = new GameObject("FPCamera");
	camera->AddComponent<Camera>()
		->CreateProjectionMatrix(0.25f * XM_PI, (float)screenWidth / screenHeight, 0.1f, 10000.0f);
	camera->SetParent(root);
	camera->SetLocalPosition(0, 2, 0);

	//Add FirstPersonMovement component
	FirstPersonMovement* fps = root->AddComponent<FirstPersonMovement>();
	root->SetPosition(0, 2.5f, 0);
	return fps;
}

//Apply movement
void FirstPersonMovement::FixedUpdate(float fixedTimestep)
{
	//Initialize physics
	XMVECTOR accVec = XMVectorSet(0, 0, 0, 0);
	XMVECTOR velVec = XMLoadFloat3(&velocity);
	prevGrounded = grounded;
	grounded = controller->IsGrounded();

	//Get correct speed mult
	float currSpeed = MOVE_SPEED;
	if (sprinting)
		currSpeed = SPRINT_SPEED;
	else if (crouching)
		currSpeed = CROUCH_SPEED;

	//Apply movement
	XMVECTOR moveVec = XMVectorSet(0, 0, 0, 0);
	//Sliding movement
	if(IsSliding())
	{
		//Apply start slide physics
		if (slideState == SlideState::Starting)
		{
			slideState = SlideState::Sliding;
			accVec = XMVectorAdd(accVec,
				XMVectorScale(XMLoadFloat3(&gameObject()->GetForwardAxis()), SLIDE_SPEED));
			slideDir = gameObject()->GetForwardAxis();
		}
		else 
		{
			//If we stopped sliding from velocity
			float velX = XMVectorGetX(velVec);
			float velZ = XMVectorGetZ(velVec);
			if (velX < SLIDE_CUTOFF && velX > -SLIDE_CUTOFF
				&& velZ < SLIDE_CUTOFF && velZ > -SLIDE_CUTOFF)
			{
				EndSlide(false);
				StartCrouch();
			}
			//Apply friction
			else accVec = XMVectorSubtract(accVec, XMVectorScale(XMLoadFloat3(&slideDir), SLIDE_FRICTION));
		}

		//Sweep to stop the slide if we hit geometry
		//Stop sliding if we hit some
		SweepHit hit;
		CollisionLayers layers = controller->GetCollisionLayers();
		layers.Unset(CollisionLayer::Player);
		Sweep(controller, slideDir, &hit, SLIDE_BODY_LENGTH, layers);
		if (hit.collider != nullptr)
		{
			EndSlide(false);
			StartCrouch();
		}
	}
	//In-air movement
	else if (!grounded)
	{
		//Relative Z movement
		//W
		if (movementZ == 1)
		{
			if (zInAirMult < 1)
				zInAirMult += fixedTimestep * ACC_JUMP_MULT;
		}
		//S
		else if (movementZ == 2)
		{
			if (zInAirMult > -1)
				zInAirMult -= fixedTimestep * ACC_JUMP_MULT;
		}
		//No input
		else
		{
			if (zInAirMult < -0.01)
				zInAirMult += fixedTimestep * ACC_JUMP_MULT;
			else if (zInAirMult > 0.01)
				zInAirMult -= fixedTimestep * ACC_JUMP_MULT;
			else zInAirMult = 0;
		}
		moveVec = XMVectorAdd(moveVec,
			XMVectorScale(
				XMLoadFloat3(&gameObject()->GetForwardAxis()), 
				JUMP_HORIZONTAL_SPEED * zInAirMult));

		//Relative X movement
		//D
		if (movementX == 1)
		{
			if (xInAirMult < 1)
				xInAirMult += fixedTimestep * ACC_JUMP_MULT;
		}
		//A
		else if (movementX == 2)
		{
			if (xInAirMult > -1)
				xInAirMult -= fixedTimestep * ACC_JUMP_MULT;
		}
		//No input
		else
		{
			if (xInAirMult < -0.01)
				xInAirMult += fixedTimestep * ACC_JUMP_MULT;
			else if (xInAirMult > 0.01)
				xInAirMult -= fixedTimestep * ACC_JUMP_MULT;
			else xInAirMult = 0;
		}
		moveVec = XMVectorAdd(moveVec,
			XMVectorScale(
				XMLoadFloat3(&gameObject()->GetRightAxis()),
				JUMP_HORIZONTAL_SPEED * xInAirMult));
	}
	//Normal movement
	else
	{
		//Relative Z movement
		//W
		if (movementZ == 1)
		{
			if (zMult < 1)
				zMult += fixedTimestep * ACC_MULT;
		}
		//S
		else if (movementZ == 2)
		{
			if(zMult > -1)
				zMult -= fixedTimestep * ACC_MULT;
		}
		//No input
		else
		{
			if (zMult < -0.01)
				zMult += fixedTimestep * ACC_MULT;
			else if (zMult > 0.01)
				zMult -= fixedTimestep * ACC_MULT;
			else zMult = 0;
		}
		moveVec = XMVectorAdd(moveVec,
			XMVectorScale(
				XMLoadFloat3(&gameObject()->GetForwardAxis()),
				currSpeed * zMult));

		//Relative X movement
		//D
		if (movementX == 1)
		{
			if (xMult < 1)
				xMult += fixedTimestep * ACC_MULT;
		}
		//A
		else if (movementX == 2)
		{
			if (xMult > -1)
				xMult -= fixedTimestep * ACC_MULT;
		}
		//No input
		else
		{
			if (xMult < -0.01)
				xMult += fixedTimestep * ACC_MULT;
			else if (xMult > 0.01)
				xMult -= fixedTimestep * ACC_MULT;
			else xMult = 0;
		}
		moveVec = XMVectorAdd(moveVec,
			XMVectorScale(
				XMLoadFloat3(&gameObject()->GetRightAxis()),
				currSpeed * xMult));
	}

	//Clamp displacement
	moveVec = XMVector3ClampLength(moveVec, 0, currSpeed);
	//Scale displacement by the timestep
	moveVec = XMVectorScale(moveVec, fixedTimestep);

	//Landing, so remove our horizontal velocity
	if (grounded && !prevGrounded)
		velVec = XMVectorSet(0, XMVectorGetY(velVec), 0, 0);

	//Apply jump physics
	if (jump)
	{
		XMVECTOR startingDir = slideState == SlideState::EndingFromJump ?
			velVec : moveVec;
		velVec = XMVectorSet(
			XMVectorGetX(startingDir),
			0,
			XMVectorGetZ(startingDir),
			0);
		accVec = XMVectorSetY(accVec, JUMP_SPEED);
		jump = false;
	}

	//Reset velocity and accelaration after end of slide
	if (slideState == SlideState::Ending)
	{
		slideState = SlideState::None;
		velVec = XMVectorSet(0, XMVectorGetY(velVec), 0, 0);
		accVec = XMVectorSet(0, XMVectorGetY(accVec), 0, 0);
	}
	else if (slideState == SlideState::EndingFromJump)
	{
		slideState = SlideState::None;
		accVec = XMVectorSet(0, XMVectorGetY(accVec), 0, 0);
	}

	//Apply acceleration to velocity
	velVec = XMVectorAdd(velVec, XMVectorScale(accVec, fixedTimestep));

	//Apply gravity to velocity
	if (applyGravity && !grounded)
		velVec = XMVectorSetY(velVec, XMVectorGetY(velVec) + (PhysicsManager::GetInstance()->GetGravity() * fixedTimestep));

	//Apply velocity to displacement
	moveVec = XMVectorAdd(moveVec, XMVectorScale(velVec, fixedTimestep));

	//Store velocity for next frame
	XMStoreFloat3(&velocity, velVec);

	//Move the character
	XMFLOAT3 move;
	XMStoreFloat3(&move, moveVec);
	controller->Move(move, fixedTimestep, false);
}

//Detect input
void FirstPersonMovement::Update(float deltaTime)
{
	//Rotate the camera to where the user is looking
	inputManager->CaptureWindow();
	CalculateCameraRotFromMouse();

	//Detect Input first
	movementZ = 0; //0=none, 1=W, 2=S
	movementX = 0; //0=none, 1=D, 2=A
	if (inputManager->GetKey(Key::W))
		movementZ = 1;
	else if (inputManager->GetKey(Key::S))
		movementZ = 2;
	if (inputManager->GetKey(Key::D))
		movementX = 1;
	else if (inputManager->GetKey(Key::A))
		movementX = 2;

	//Sprint check
	//Don't have to hold the sprint buttons down
	//Must be moving forward
	//Toggle
	if (grounded && inputManager->GetKeyDown(Key::LeftShift) && !sprinting && movementZ == 1)
	{
		if (crouching)
			EndCrouch();
		else if (IsSliding())
			EndSlide(false);
		
		StartSprint();
	}
	else if (sprinting && movementZ == 0)
		EndSprint();

	//Crouch check
	//Toggle
	if (grounded && inputManager->GetKeyDown(Key::LeftControl))
	{
		//If we are sprinting, start a slide
		if (crouching)
		{
			EndCrouch();
		}
		else if (sprinting)
		{
			EndSprint();
			StartSlide();
		}
		else if (IsSliding())
		{
			EndSlide(false);
			StartCrouch();
		}
		else StartCrouch();
	}

	//Jumping
	if (grounded && inputManager->GetKeyDown(Key::Space))
	{
		if (crouching)
			EndCrouch();
		else if (IsSliding())
			EndSlide(true);
		jump = true;
	}
}

// Changes for when we start a sprint
void FirstPersonMovement::StartSprint()
{
	sprinting = true;
}
// Changes for when we end a sprint
void FirstPersonMovement::EndSprint()
{
	sprinting = false;
}

// Changes for when we start a crouch
void FirstPersonMovement::StartCrouch()
{
	crouching = true;
	controller->Resize(CROUCH_HEIGHT);
	cameraGO->SetLocalPosition(0, CROUCH_HEIGHT, 0);
}
// Changes for when we end a crouch
void FirstPersonMovement::EndCrouch()
{
	crouching = false;
	controller->Resize(STAND_HEIGHT);
	cameraGO->SetLocalPosition(0, STAND_HEIGHT, 0);
}

// Changes for when we start a slide
void FirstPersonMovement::StartSlide()
{
	slideState = SlideState::Starting;
	controller->Resize(SLIDE_HEIGHT);
	cameraGO->SetLocalPosition(0, SLIDE_HEIGHT, 0);

}
// Changes for when we end a slide
void FirstPersonMovement::EndSlide(bool fromJump)
{
	if (fromJump)
		slideState = SlideState::EndingFromJump;
	else slideState = SlideState::Ending;
	controller->Resize(STAND_HEIGHT);
	cameraGO->SetLocalPosition(0, STAND_HEIGHT, 0);
}
// If the slide state is in a valid sliding state
bool FirstPersonMovement::IsSliding()
{
	return slideState == SlideState::Starting || slideState == SlideState::Sliding;
}

// Calculate the camera's rotation when the player moves the mouse
void FirstPersonMovement::CalculateCameraRotFromMouse()
{
	static float cameraSensitivity = 0.15f;

	//Get the center position of the window
	long centerX = inputManager->GetWindowCenterX();
	long centerY = inputManager->GetWindowCenterY();

	//Get the mouse position
	long mouseX = inputManager->GetMouseX();
	long mouseY = inputManager->GetMouseY();

	//Calculate the difference in view with the angle
	float fAngleX = 0.0f;
	float fAngleY = 0.0f;
	float fDeltaMouse = 0.0f;
	if (mouseX > centerX)
	{
		fDeltaMouse = static_cast<float>(mouseX - centerX);
		fAngleY += fDeltaMouse * cameraSensitivity;
	}
	else if (mouseX < centerX)
	{
		fDeltaMouse = static_cast<float>(centerX - mouseX);
		fAngleY -= fDeltaMouse * cameraSensitivity;
	}

	if (mouseY > centerY)
	{
		fDeltaMouse = static_cast<float>(centerY - mouseY);
		fAngleX -= fDeltaMouse * cameraSensitivity;
	}
	else if (mouseY < centerY)
	{
		fDeltaMouse = static_cast<float>(mouseY - centerY);
		fAngleX += fDeltaMouse * cameraSensitivity;
	}

	static float xRot = 0;

	xRot += fAngleX;
	yRot += fAngleY;

	//Keep camera from reversing when looking up/down
	if (xRot > 89.9f)
		xRot = 89.9f;
	if (xRot < -89.9f)
		xRot = -89.9f;

	//Change the Yaw and the Pitch of the camera
	gameObject()->SetRotation(0, yRot, 0);
	cameraGO->SetRotation(xRot, yRot, 0);

	SetCursorPos(centerX, centerY); //Position the mouse in the center
	SetCursor(false);
}

// Get the controller's camera
Camera* FirstPersonMovement::GetCamera()
{
	return camera;
}
