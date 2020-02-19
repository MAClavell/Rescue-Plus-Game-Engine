#include "FirstPersonMovement.h"
#include "ExtendedMath.h"
#include "PhysicsManager.h"
#include "Raycast.h"

using namespace DirectX;

//Speeds
#define MOVE_SPEED 12
#define SPRINT_SPEED 16
#define CROUCH_SPEED 5
#define IN_AIR_ACC 30
#define JUMP_SPEED 500
#define ACC_MULT 6
#define ACC_IN_AIR_MULT 4
#define SLIDE_FRICTION (SPRINT_SPEED * 0.5f)
#define SLIDE_CUTOFF 1.0f

//Heights
#define STAND_HEIGHT 2.0f
#define CROUCH_HEIGHT 1.5f
#define SLIDE_HEIGHT 1.0f
#define SLIDE_BODY_LENGTH 3.0f

//Camera
#define WALK_BOB_MAX 0.05f
#define WALK_BOB_SPEED 5.0f
#define SPRINT_BOB_MAX 0.075f
#define SPRINT_BOB_SPEED 8.0f
#define CROUCH_BOB_MAX 0.075f
#define CROUCH_BOB_SPEED 6.0f

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
	cameraState = CameraState::None;
	movementX = 0;
	movementZ = 0;
	xMult = 0;
	zMult = 0;
	inAirMaxSpeed = MOVE_SPEED;

	velocity = XMFLOAT3(0, 0, 0);
	slideDir = XMFLOAT3(0, 0, 0);

	lastFrameCameraPos = cameraGO->GetPosition();
	cameraPos = XMFLOAT3(0, STAND_HEIGHT, 0);
	cameraTargetPos = XMFLOAT3(0, STAND_HEIGHT, 0);
	cameraT = 0;
	cameraDir = 1;
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
	camera->SetLocalPosition(0, STAND_HEIGHT, 0);

	//Add FirstPersonMovement component
	FirstPersonMovement* fps = root->AddComponent<FirstPersonMovement>();
	root->SetPosition(0, 2.5f, 0);
	return fps;
}

//Apply movement
void FirstPersonMovement::FixedUpdate(float fixedTimestep)
{
	lastFrameCameraPos = cameraGO->GetPosition();

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
	else if (!grounded)
		currSpeed = IN_AIR_ACC;

	//Get the correct max speed
	float maxSpeed = currSpeed;
	if (IsSliding())
		maxSpeed = SPRINT_SPEED * SPRINT_SPEED;
	if (!grounded)
		maxSpeed = inAirMaxSpeed;

	//Apply movement
	//Sliding movement
	if(IsSliding())
	{
		//Apply start slide physics
		if (slideState == SlideState::Starting)
		{
			slideState = SlideState::Sliding;
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
	//Normal movement
	else
	{
		XMVECTOR moveVec = XMVectorSet(0, 0, 0, 0);
		float mult = ACC_MULT;
		if (!grounded)
			mult = ACC_IN_AIR_MULT;

		//Relative Z movement
		//W
		if (movementZ == 1)
		{
			if (zMult < 1)
				zMult += fixedTimestep * mult;
		}
		//S
		else if (movementZ == 2)
		{
			if(zMult > -1)
				zMult -= fixedTimestep * mult;
		}
		//No input
		else
		{
			if (zMult < -0.05)
				zMult += fixedTimestep * mult;
			else if (zMult > 0.05)
				zMult -= fixedTimestep * mult;
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
				xMult += fixedTimestep * mult;
		}
		//A
		else if (movementX == 2)
		{
			if (xMult > -1)
				xMult -= fixedTimestep * mult;
		}
		//No input
		else
		{
			if (xMult < -0.05)
				xMult += fixedTimestep * mult;
			else if (xMult > 0.05)
				xMult -= fixedTimestep * mult;
			else xMult = 0;
		}
		moveVec = XMVectorAdd(moveVec,
			XMVectorScale(
				XMLoadFloat3(&gameObject()->GetRightAxis()),
				currSpeed * xMult));

		//Set velocity
		if (!grounded)
			accVec = XMVectorSet(XMVectorGetX(moveVec), XMVectorGetY(accVec), XMVectorGetZ(moveVec), 0);
		else velVec = XMVectorSet(XMVectorGetX(moveVec), XMVectorGetY(velVec), XMVectorGetZ(moveVec), 0);
	}

	//Landing, so remove our horizontal velocity
	if (grounded && !prevGrounded)
		velVec = XMVectorSet(0, XMVectorGetY(velVec), 0, 0);

	//Apply jump physics
	if (jump)
	{
		velVec = XMVectorSetY(velVec, 0);
		accVec = XMVectorSetY(accVec, JUMP_SPEED);
		jump = false;
		inAirMaxSpeed = maxSpeed;
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

	//Clamp velocity on the horizontal axis
	float y = XMVectorGetY(velVec);
	velVec = XMVectorSetY(velVec, 0);
	velVec = XMVector3ClampLength(velVec, 0, maxSpeed);
	velVec = XMVectorSetY(velVec, y);

	//Apply gravity to velocity
	if (applyGravity && !grounded)
		velVec = XMVectorSetY(velVec, XMVectorGetY(velVec) + (PhysicsManager::GetInstance()->GetGravity() * fixedTimestep));

	//Apply velocity to displacement
	XMVECTOR displacementVec = XMVectorSet(0, 0, 0, 0);
	displacementVec = XMVectorAdd(displacementVec, XMVectorScale(velVec, fixedTimestep));

	//Store velocity for next frame
	XMStoreFloat3(&velocity, velVec);

	//Move the character
	XMFLOAT3 displacement;
	XMStoreFloat3(&displacement, displacementVec);
	controller->Move(displacement, fixedTimestep, false);

	ApplyCameraEffects(fixedTimestep);
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

	//Camera states for none and walking
	if (grounded && !jump && !IsSliding() && !sprinting && !crouching)
	{
		if (movementX == 0 && movementZ == 0 && cameraState != CameraState::None)
			cameraState = CameraState::None;
		else if ((movementX != 0 || movementZ != 0) && cameraState != CameraState::Walking)
		{
			cameraTargetPos = XMFLOAT3(0, STAND_HEIGHT + WALK_BOB_MAX, 0);
			cameraState = CameraState::Walking;
			cameraT = 0;
			cameraDir = 1;
		}
	}
}

// Apply various effects to the camera depending on the movement state
void FirstPersonMovement::ApplyCameraEffects(float fixedTimestep)
{
	float speed = 0;
	//Walking
	if (cameraState == CameraState::Walking)
		speed = WALK_BOB_SPEED;
	//Sprinting
	else if (cameraState == CameraState::Sprinting)
		speed = SPRINT_BOB_SPEED;
	//Crouching
	else if (cameraState == CameraState::Crouching)
		speed = CROUCH_BOB_SPEED;

	//Head bobbing
	if ((movementX != 0 || movementZ != 0) &&
		(cameraState == CameraState::Walking || cameraState == CameraState::Sprinting || cameraState == CameraState::Crouching))
	{
		cameraT += cameraDir * speed * fixedTimestep;
		if (cameraT > 1)
			cameraDir = -1;
		else if (cameraT < 0)
			cameraDir = 1;

		XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMVectorLerp(XMLoadFloat3(&cameraPos), XMLoadFloat3(&cameraTargetPos), cameraT));
		cameraGO->SetLocalPosition(pos);
	}
}

// Changes for when we start a sprint
void FirstPersonMovement::StartSprint()
{
	sprinting = true;
	cameraTargetPos = XMFLOAT3(0, STAND_HEIGHT + SPRINT_BOB_MAX, 0);
	cameraState = CameraState::Sprinting;
	cameraT = 0;
	cameraDir = 1;
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
	cameraTargetPos = XMFLOAT3(0, STAND_HEIGHT + CROUCH_BOB_MAX, 0);
	cameraState = CameraState::Crouching;
	cameraT = 0;
	cameraDir = 1;
}
// Changes for when we end a crouch
void FirstPersonMovement::EndCrouch()
{
	crouching = false;
	controller->Resize(STAND_HEIGHT);
}

// Changes for when we start a slide
void FirstPersonMovement::StartSlide()
{
	cameraState = CameraState::Sliding;
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
	double fAngleX = 0.0f;
	double fAngleY = 0.0f;
	double fDeltaMouse = 0.0f;
	if (mouseX > centerX)
	{
		fDeltaMouse = static_cast<double>(mouseX - centerX);
		fAngleY += fDeltaMouse * cameraSensitivity;
	}
	else if (mouseX < centerX)
	{
		fDeltaMouse = static_cast<double>(centerX - mouseX);
		fAngleY -= fDeltaMouse * cameraSensitivity;
	}

	if (mouseY > centerY)
	{
		fDeltaMouse = static_cast<double>(centerY - mouseY);
		fAngleX -= fDeltaMouse * cameraSensitivity;
	}
	else if (mouseY < centerY)
	{
		fDeltaMouse = static_cast<double>(mouseY - centerY);
		fAngleX += fDeltaMouse * cameraSensitivity;
	}

	static double xRot = 0;
	static double yRot = 0;

	xRot += fAngleX;
	yRot += fAngleY;

	//Keep camera from reversing when looking up/down
	//Clamp when sliding
	if (IsSliding())
		xRot = Clamp(xRot, -89.99f, 10.0f);
	else
		xRot = Clamp(xRot, -89.99f, 89.99f);

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
