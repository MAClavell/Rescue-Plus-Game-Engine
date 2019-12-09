#include "FirstPersonMovement.h"

using namespace DirectX;

FirstPersonMovement::FirstPersonMovement(GameObject* gameObject) : UserComponent(gameObject)
{
	inputManager = InputManager::GetInstance();
	auto children = gameObject->GetChildren();

	//Get Camera
	camera = children[0]->GetComponent<Camera>();

	//Get rigidbody
	rb = gameObject->GetComponent<RigidBody>();

	//Get colliders
	standCol = gameObject->GetComponent<CapsuleCollider>();
	crouchCol = children[1]->GetComponent<CapsuleCollider>();
	slideCol = children[2]->GetComponent<CapsuleCollider>();
}
FirstPersonMovement::~FirstPersonMovement()
{ }

// Factory function to safely create a First Person Movement object
FirstPersonMovement* FirstPersonMovement::CreateFirstPersonCharacter(const char* name)
{
	//Root object
	GameObject* root = new GameObject(name);
	root->AddComponent<RigidBody>(1.0f);
	root->AddComponent<CapsuleCollider>();

	//Camera object
	GameObject* camera = new GameObject("FPCamera");
	camera->AddComponent<Camera>();
	camera->SetParent(root);

	//Crouching collider
	GameObject* crouchCol = new GameObject("CrouchingCollider");
	crouchCol->AddComponent<CapsuleCollider>();
	crouchCol->SetParent(root);

	//Sliding collider
	GameObject* slideCol = new GameObject("SlidingCollider");
	slideCol->AddComponent<CapsuleCollider>();
	slideCol->SetParent(root);

	//Add FirstPersonMovement component
	FirstPersonMovement* fps = root->AddComponent<FirstPersonMovement>();

	return fps;
}

void FirstPersonMovement::Update(float deltaTime)
{
	//Detect Input in the update loop

	//Don't have to hold these buttons down
	if (inputManager->GetKey('Q') && !sprinting)
	{
		if (crouching)
			EndCrouch();
		if (sliding)
			EndSlide();

		StartSprint();
	}
	if (inputManager->GetKey('C') && !crouching)
	{
		//If we are sprinting, start a slide
		if (sprinting)
		{
			EndSprint();
			StartSlide();
		}
		else if (sliding)
		{
			EndSlide();
			StartCrouch();
		}
		else StartCrouch();
	}

	//Get correct speed mult
	float speedMult = 1;
	if (sprinting)
		speedMult = 2;
	else if (crouching)
		speedMult = 0.5f;
	//else if (sliding)

	//Normal movement
	if (!sliding)
	{
		XMFLOAT3 velocity;
		//Relative Z movement
		if (inputManager->GetKey('W'))
		{
			velocity.z = baseSpeed * speedMult;
		}
		else if (inputManager->GetKey('S'))
		{
			velocity.z = -baseSpeed * speedMult;
		}
		//Relative X movement
		if (inputManager->GetKey('D'))
		{
			velocity.x += baseSpeed * speedMult;
		}
		else if (inputManager->GetKey('A'))
		{
			velocity.x = -baseSpeed * speedMult;
		}
	}
	//Sliding movement
	else
	{

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
}
// Changes for when we end a crouch
void FirstPersonMovement::EndCrouch()
{
	crouching = false;
}

// Changes for when we start a slide
void FirstPersonMovement::StartSlide()
{
	sliding = true;
}
// Changes for when we end a slide
void FirstPersonMovement::EndSlide()
{
	sliding = false;
}