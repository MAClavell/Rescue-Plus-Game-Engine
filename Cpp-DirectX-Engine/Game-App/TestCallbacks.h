#pragma once
#include "Component.h"
class TestCallbacks :
	public UserComponent
{
public:
	TestCallbacks(GameObject* gameObject);
	~TestCallbacks();

	void OnCollisionEnter(Collision collision) override;
	void OnCollisionStay(Collision collision) override;
	void OnCollisionExit(Collision collision) override;

};

