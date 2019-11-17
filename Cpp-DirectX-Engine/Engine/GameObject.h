#pragma once
#include <DirectXMath.h>
#include <vector>
#include <string>

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
	// Get the GameObject this component is tied to
	// --------------------------------------------------------
	GameObject* gameObject() { return attatchedGameObject; }
};

// --------------------------------------------------------
// A GameObject definition.
//
// A GameObject contains world data
// --------------------------------------------------------
class GameObject
{
private:
	//Parenting
	GameObject* parent;
	std::vector<GameObject*> children;

	//Transformations
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInvTrans;

	DirectX::XMFLOAT3 forwardAxis;
	DirectX::XMFLOAT3 rightAxis;
	DirectX::XMFLOAT3 upAxis;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 localPosition;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT4 localRotation;
	DirectX::XMFLOAT3 scale;
	bool worldDirty;

	//Components
	std::vector<Component*> components;

	// --------------------------------------------------------
	// Remove a child from a gameobject
	// --------------------------------------------------------
	void RemoveChild(GameObject* child);

	// --------------------------------------------------------
	// Add a child to a gameobject
	// --------------------------------------------------------
	void AddChild(GameObject* child);
	
	// --------------------------------------------------------
	// Set the position for this GameObject
	//
	// newPosition - The new position to go to
	// fromRigidBody - If this position setting is from a rigidbody
	// --------------------------------------------------------
	void SetPosition(DirectX::XMFLOAT3 newPosition, bool setLocal, bool fromRigidBody = false);

	// --------------------------------------------------------
	// Set the rotation for this GameObject (Quaternion)
	//
	// newQuatRotation - The new rotation to rotate to
	// fromRigidBody - If this position setting is from a rigidbody
	// --------------------------------------------------------
	void SetRotation(DirectX::XMFLOAT4 newQuatRotation, bool setLocal, bool fromRigidBody = false);

	// --------------------------------------------------------
	// Update transformations after parent's transformations changed
	// --------------------------------------------------------
	//void ParentPositionChanged();

	// --------------------------------------------------------
	// Update transformations after parent's transformations changed
	// --------------------------------------------------------
	void ParentRotationChanged();
	
	// --------------------------------------------------------
	// Update transformations after parent's transformations changed
	// --------------------------------------------------------
	void ParentScaleChanged();

	// --------------------------------------------------------
	// Calculate the local axis for the gameobject
	// --------------------------------------------------------
	void CalculateAxis();

protected:
	bool enabled;
	std::string name;

public:

	// --------------------------------------------------------
	// Constructor - Set up the gameobject.
	// --------------------------------------------------------
	GameObject();

	// --------------------------------------------------------
	// Constructor - Set up the gameobject.
	//
	// name - the name of the gameobject
	// --------------------------------------------------------
	GameObject(std::string name);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted.
	// Destroys all children too
	// --------------------------------------------------------
	~GameObject();

	// --------------------------------------------------------
	// Get the enabled state of the gameobject
	// Disabled objects are not updated or drawn
	// --------------------------------------------------------
	bool GetEnabled();

	// --------------------------------------------------------
	// Enable or disable the gameobject
	// Disabled objects are not updated or drawn
	// --------------------------------------------------------
	void SetEnabled(bool enabled);

	// --------------------------------------------------------
	// Set the name of this gameobject
	// --------------------------------------------------------
	void SetName(std::string name);

	// --------------------------------------------------------
	// Get the name of this gameobject
	// --------------------------------------------------------
	std::string GetName();

	// --------------------------------------------------------
	// Set the parent of this GameObject
	// --------------------------------------------------------
	void SetParent(GameObject* parent);
	
	// --------------------------------------------------------
	// Get the parent of this GameObject
	// --------------------------------------------------------
	GameObject* GetParent();

	// --------------------------------------------------------
	// Add a component of a specific type (must derive from component)
	// --------------------------------------------------------
	template <typename T, typename... Args>
	T* AddComponent(Args... args)
	{
		static_assert(std::is_base_of<Component, T>::value, "Can't add a component not derived from Component\n");

		//Push new T
		T* component = new T(this, args...);
		components.push_back(component);
		return component;
	}

	// --------------------------------------------------------
	// Get a component of a specific type (must derive from component
	//		and be in the gameobject's component list)
	// --------------------------------------------------------
	template <typename T>
	T* GetComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "Can't remove a component not derived from Component\n");

		//Try to find it
		for (auto iter = components.begin(); iter != components.end(); iter++)
		{
			try
			{
				T* c = dynamic_cast<T*>(*iter); // try to cast
				if(c)
					return c;
			}
			catch (...)
			{	}
		}

		return nullptr;
	}

	// --------------------------------------------------------
	// Remove a component of a specific type (must derive from component
	//		and be in the gameobject's component list)
	// --------------------------------------------------------
	template <typename T>
	void RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "Can't remove a component not derived from Component\n");

		//Try to find it
		bool found = false;
		for (auto iter = components.begin(); iter != components.end(); iter++)
		{
			try
			{
				T& c = dynamic_cast<T&>(**iter); // try to cast
				found = true;

				//Swap with the last and pop
				std::iter_swap(iter, components.end());
				components.pop_back();

				//Delete
				delete c;
				break;
			}
			catch (...)
			{
			}
		}

		if (!found)
			printf("Could not find a component of type '%s' in %s\n", typeid(T).name(), name.c_str());
	}

	// --------------------------------------------------------
	// Update all componenets in this gameObject
	// --------------------------------------------------------
	virtual void Update(float deltaTime);

	// --------------------------------------------------------
	// Get the world matrix for this GameObject (rebuilding if necessary)
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	// --------------------------------------------------------
	// Get the inverse transpose of the world matrix for this GameObject 
	// (rebuilding if necessary)
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetWorldInvTransMatrix();

	// --------------------------------------------------------
	// Rebuild the world matrix from the different components
	// --------------------------------------------------------
	void RebuildWorld();

	// --------------------------------------------------------
	// Get the position for this GameObject
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetPosition();

	// --------------------------------------------------------
	// Get the local position for this GameObject
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetLocalPosition();

	// --------------------------------------------------------
	// CALLED BY ENGINE FUNCTIONS ONLY
	// Set the position for this GameObject from a rigidbody
	//
	// newPosition - The new position to go to
	// --------------------------------------------------------
	void SetPositionFromRigidBody(DirectX::XMFLOAT3 newPosition);

	// --------------------------------------------------------
	// Set the position for this GameObject
	//
	// newPosition - The new position to go to
	// --------------------------------------------------------
	void SetPosition(DirectX::XMFLOAT3 newPosition);

	// --------------------------------------------------------
	// Set the position for this GameObject
	//
	// x - new x position
	// y - new y position
	// z - new z position
	// --------------------------------------------------------
	void SetPosition(float x, float y, float z);

	// --------------------------------------------------------
	// Set the local position for this GameObject
	//
	// newLocalPosition - The new local position to go to
	// --------------------------------------------------------
	void SetLocalPosition(DirectX::XMFLOAT3 newLocalPosition);

	// --------------------------------------------------------
	// Set the local position for this GameObject
	//
	// x - new x position
	// y - new y position
	// z - new z position
	// --------------------------------------------------------
	void SetLocalPosition(float x, float y, float z);

	// --------------------------------------------------------
	// Moves this GameObject in absolute space by a given vector.
	// Does not take rotation into account
	//
	// moveTo - The amount to move for each axis
	// --------------------------------------------------------
	void MoveAbsolute(DirectX::XMFLOAT3 moveAmnt);

	// --------------------------------------------------------
	// Moves this GameObject in relative space by a given vector.
	// Does take rotation into account
	//
	// moveTo - The amount to move for each axis
	// --------------------------------------------------------
	void MoveRelative(DirectX::XMFLOAT3 moveAmnt);

	// --------------------------------------------------------
	// Get the rotated forward axis of this gameobject
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetForwardAxis();

	// --------------------------------------------------------
	// Get the rotated right axis of this gameobject
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetRightAxis();

	// --------------------------------------------------------
	// Get the rotated up axis of this gameobject
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetUpAxis();

	// --------------------------------------------------------
	// Get the rotation for this GameObject (Quaternion)
	// --------------------------------------------------------
	DirectX::XMFLOAT4 GetRotation();

	// --------------------------------------------------------
	// Get the local rotation for this GameObject (Quaternion)
	// --------------------------------------------------------
	DirectX::XMFLOAT4 GetLocalRotation();

	// --------------------------------------------------------
	// CALLED BY ENGINE FUNCTIONS ONLY
	// Set the rotation for this GameObject (Quaternion) from a rigidbody
	//
	// newQuatRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetRotationFromRigidBody(DirectX::XMFLOAT4 newQuatRotation);

	// --------------------------------------------------------
	// Set the rotation for this GameObject (Quaternion)
	//
	// newQuatRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetRotation(DirectX::XMFLOAT4 newQuatRotation);

	// --------------------------------------------------------
	// Set the rotation for this GameObject (Angles)
	//
	// newRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetRotation(DirectX::XMFLOAT3 newRotation);

	// --------------------------------------------------------
	// Set the rotation for this GameObject using angles
	//
	// x - x angle
	// y - y angle
	// z - z angle
	// --------------------------------------------------------
	void SetRotation(float x, float y, float z);

	// --------------------------------------------------------
	// Rotate this GameObject (Angles)
	//
	// newRotation - The vector rotation
	// --------------------------------------------------------
	void Rotate(DirectX::XMFLOAT3 newRotation);

	// --------------------------------------------------------
	// Rotate this GameObject using angles
	//
	// x - x angle
	// y - y angle
	// z - z angle
	// --------------------------------------------------------
	void Rotate(float x, float y, float z);

	// --------------------------------------------------------
	// Set the local rotation for this GameObject (Quaternion)
	//
	// newQuatRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetLocalRotation(DirectX::XMFLOAT4 newLocalQuatRotation);

	// --------------------------------------------------------
	// Set the local rotation for this GameObject (Angles)
	//
	// newRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetLocalRotation(DirectX::XMFLOAT3 newLocalRotation);

	// --------------------------------------------------------
	// Set the local rotation for this GameObject using angles
	//
	// x - x angle
	// y - y angle
	// z - z angle
	// --------------------------------------------------------
	void SetLocalRotation(float x, float y, float z);

	// --------------------------------------------------------
	// Get the scale for this GameObject
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetScale();

	// --------------------------------------------------------
	// Set the scale for this GameObject
	//
	// newScale - The new scale to resize to
	// --------------------------------------------------------
	void SetScale(DirectX::XMFLOAT3 newScale);

	// --------------------------------------------------------
	// Set the scale for this GameObject
	//
	// x - new x scale
	// y - new y scale
	// z - new z scale
	// --------------------------------------------------------
	void SetScale(float x, float y, float z);
};

