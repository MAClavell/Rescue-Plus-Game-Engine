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

	void OnTriggerEnter(Collision collision) override;
	void OnTriggerStay(Collision collision) override;
	void OnTriggerExit(Collision collision) override;

};

