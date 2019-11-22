#pragma once
#include "Component.h"
class ShipyardCrane :
	public UserComponent
{
private:
	GameObject* hook;

public:
	ShipyardCrane(GameObject* gameObject, GameObject* hook);
	~ShipyardCrane();

	void Update(float deltaTime) override;
};

