#pragma once

class GameObject;
class Component
{
private:
	GameObject* attatchedGameObject;

public:
	// --------------------------------------------------------
	//Construct a component
	// --------------------------------------------------------
	Component(GameObject* gameObject);

	// --------------------------------------------------------
	// Destroy a component
	// --------------------------------------------------------
	virtual ~Component() { }

	// --------------------------------------------------------
	// Update this component
	// --------------------------------------------------------
	virtual void Update(float deltaTime) {};

	// --------------------------------------------------------
	// Update this component at the fixed timestep
	// --------------------------------------------------------
	virtual void FixedUpdate(float deltaTime) {};

	// --------------------------------------------------------
	// Get the GameObject this component is tied to
	// --------------------------------------------------------
	GameObject* gameObject() { return attatchedGameObject; }
};

struct Collision;
class UserComponent : public Component
{
public:
	// --------------------------------------------------------
	//Construct a component
	// --------------------------------------------------------
	UserComponent(GameObject* gameObject) : Component(gameObject) { };

	// --------------------------------------------------------
	// Destroy a component
	// --------------------------------------------------------
	virtual ~UserComponent() { };

	// --------------------------------------------------------
	// Override for a callback when a physics object collides with this
	// --------------------------------------------------------
	virtual void OnCollisionEnter(Collision collision);

	// --------------------------------------------------------
	// Override for a callback when a physics object stays collided to this
	// --------------------------------------------------------
	virtual void OnCollisionStay(Collision collision);

	// --------------------------------------------------------
	// Override for a callback when a physics object stops colliding with this
	// --------------------------------------------------------
	virtual void OnCollisionExit(Collision collision);

	// --------------------------------------------------------
	// Override for a callback when a physics object collides with this
	// --------------------------------------------------------
	virtual void OnTriggerEnter(Collision collision);

	// --------------------------------------------------------
	// Override for a callback when a physics object stays collided to this
	// --------------------------------------------------------
	virtual void OnTriggerStay(Collision collision);

	// --------------------------------------------------------
	// Override for a callback when a physics object stops colliding with this
	// --------------------------------------------------------
	virtual void OnTriggerExit(Collision collision);
};