#pragma once
#include <DirectXMath.h>
#include "Component.h"
#include <vector>
#include <string>
#include "Messenger.hpp"
#include "MiscHelpers.h"

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
	DirectX::XMFLOAT4X4 worldRaw;
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

	//Messengers
	Messenger<DirectX::XMFLOAT3, bool, bool> onPositionChanged;
	Messenger<DirectX::XMFLOAT4, bool, bool> onRotationChanged;
	Messenger<DirectX::XMFLOAT3> onScaleChanged;

	//Components
	std::vector<Component*> components;
	//Make separate lists for components that implement
	// each overrideable function. This prevents looping
	// and function calls on components that don't implement
	// a certain overrideable function
	std::vector<Component*> updateComponents;
	std::vector<Component*> fixedUpdateComponents;
	std::vector<UserComponent*> onControllerCollisionComponents;
	std::vector<UserComponent*> onCollisionEnterComponents;
	std::vector<UserComponent*> onCollisionStayComponents;
	std::vector<UserComponent*> onCollisionExitComponents;
	std::vector<UserComponent*> onTriggerEnterComponents;
	std::vector<UserComponent*> onTriggerStayComponents;
	std::vector<UserComponent*> onTriggerExitComponents;

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
	// --------------------------------------------------------
	void SetPosition(DirectX::XMFLOAT3 newPosition, bool setLocal,
		bool fromParent = false, bool fromPhysics = false);

	// --------------------------------------------------------
	// Set the rotation for this GameObject (Quaternion)
	//
	// newQuatRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetRotation(DirectX::XMFLOAT4 newQuatRotation, bool setLocal,
		bool fromParent = false, bool fromPhysics = false);

	// --------------------------------------------------------
	// Set the local position for this GameObject
	//
	// newLocalPosition - The new local position to go to
	// --------------------------------------------------------
	void SetLocalPosition(DirectX::XMFLOAT3 newLocalPosition, bool fromParent);

	// --------------------------------------------------------
	// Set the local rotation for this GameObject (Quaternion)
	//
	// newQuatRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetLocalRotation(DirectX::XMFLOAT4 newLocalQuatRotation, bool fromParent);

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
	// Get the children of this gameobject
	// --------------------------------------------------------
	std::vector<GameObject*> GetChildren();

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

		//Update and fixed update
		if (&Component::Update != &T::Update)
			updateComponents.push_back(component);
		if (&Component::FixedUpdate != &T::FixedUpdate)
			fixedUpdateComponents.push_back(component);

		//User componenets
		if constexpr(std::is_base_of<UserComponent, T>())
		{
			//Controller collisions
			if (&UserComponent::OnControllerCollision != &T::OnControllerCollision)
				onControllerCollisionComponents.push_back(component);

			//Collisions
			if (&UserComponent::OnCollisionEnter != &T::OnCollisionEnter)
				onCollisionEnterComponents.push_back(component);
			if (&UserComponent::OnCollisionStay != &T::OnCollisionStay)
				onCollisionStayComponents.push_back(component);
			if (&UserComponent::OnCollisionExit != &T::OnCollisionExit)
				onCollisionExitComponents.push_back(component);

			//Triggers
			if (&UserComponent::OnTriggerEnter != &T::OnTriggerEnter)
				onTriggerEnterComponents.push_back(component);
			if (&UserComponent::OnTriggerStay != &T::OnTriggerStay)
				onTriggerStayComponents.push_back(component);
			if (&UserComponent::OnTriggerExit != &T::OnTriggerExit)
				onTriggerExitComponents.push_back(component);
		}

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
		T* c;
		bool found = RemoveTypeFromVector(&components, &c);
		
		if (found)
		{
			//Remove update and fixed update
			if (&Component::Update != &T::Update)
				RemoveTypeFromVector(&updateComponents);
			if (&Component::FixedUpdate != &T::FixedUpdate)
				RemoveTypeFromVector(&fixedUpdateComponents);

			//Remove user components
			if constexpr (std::is_base_of<UserComponent, T>())
			{
				//Collisions
				if (&UserComponent::OnCollisionEnter != &T::OnCollisionEnter)
					RemoveTypeFromVector(&OnCollisionEnterComponents);
				if (&UserComponent::OnCollisionStay != &T::OnCollisionStay)
					RemoveTypeFromVector(&OnCollisionStayComponents);
				if (&UserComponent::OnCollisionExit != &T::OnCollisionExit)
					RemoveTypeFromVector(&OnCollisionExitComponents);

				//Triggers
				if (&UserComponent::OnTriggerEnter != &T::OnTriggerEnter)
					RemoveTypeFromVector(&OnTriggerEnterComponents);
				if (&UserComponent::OnTriggerStay != &T::OnTriggerStay)
					RemoveTypeFromVector(&OnTriggerStayComponents);
				if (&UserComponent::OnTriggerExit != &T::OnTriggerExit)
					RemoveTypeFromVector(&OnTriggerExitComponents);
			}

			//Delete
			delete c;
		}
#if defined(DEBUG) || defined(_DEBUG)
		else 
			printf("Could not find a component of type '%s' in %s\n", typeid(T).name(), name.c_str());
#endif // DEBUG
	}

	// --------------------------------------------------------
	// Get lists of all user components that have collision or
	// trigger callbacks
	// --------------------------------------------------------
	void GetCollisionAndTriggerCallbackComponents(
		std::vector<UserComponent*>* cntrlr,
		std::vector<UserComponent*>* colEnt,
		std::vector<UserComponent*>* colSty,
		std::vector<UserComponent*>* colExt,
		std::vector<UserComponent*>* trigEnt,
		std::vector<UserComponent*>* trigSty,
		std::vector<UserComponent*>* trigExt);

	// --------------------------------------------------------
	// Update all componenets in this gameObject
	// --------------------------------------------------------
	void Update(float deltaTime);

	// --------------------------------------------------------
	// Update all componenets in this gameObject
	// --------------------------------------------------------
	void FixedUpdate(float deltaTime);

	// --------------------------------------------------------
	// Get the world matrix for this GameObject (rebuilding if necessary)
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	// --------------------------------------------------------
	// Get the RAW world matrix for this GameObject (rebuilding if necessary)
	// DirectX requires a transposed version of the world matrix,
	//	this is the untransposed version used for Rescue+ math
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetRawWorldMatrix();

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
	// Add a listener to onPositionChanged
	// --------------------------------------------------------
	void AddListenerOnPositionChanged(std::function<void(DirectX::XMFLOAT3, bool, bool)> function);
	// --------------------------------------------------------
	// Remove a listener from onPositionChanged
	// --------------------------------------------------------
	void RemoveListenerOnPositionChanged(std::function<void(DirectX::XMFLOAT3, bool, bool)> function);

	// --------------------------------------------------------
	// Add a listener to onRotationChanged
	// --------------------------------------------------------
	void AddListenerOnRotationChanged(std::function<void(DirectX::XMFLOAT4, bool, bool)> function);
	// --------------------------------------------------------
	// Remove a listener from onRotationChanged
	// --------------------------------------------------------
	void RemoveListenerOnRotationChanged(std::function<void(DirectX::XMFLOAT4, bool, bool)> function);

	// --------------------------------------------------------
	// Add a listener to onScaleChanged
	// --------------------------------------------------------
	void AddListenerOnScaleChanged(std::function<void(DirectX::XMFLOAT3)> function);
	// --------------------------------------------------------
	// Remove a listener from onScaleChanged
	// --------------------------------------------------------
	void RemoveListenerOnScaleChanged(std::function<void(DirectX::XMFLOAT3)> function);

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
	void SetPositionFromPhysics(DirectX::XMFLOAT3 newPosition);

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
	void SetRotationFromPhysics(DirectX::XMFLOAT4 newQuatRotation);

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

