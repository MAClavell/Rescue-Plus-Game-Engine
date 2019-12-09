#include "CollisionResolver.h"
#include "Component.h"

// Send a trigger to the resolver and have it decide what events to run.
// Since PhysX doesn't have PxTriggerPairFlags for enter and exit,
// we have to manually check if a collision is entering or exiting
void CollisionResolver::SendTriggerCollision(Collision collision)
{
	bool found = false;

	//Try to find in enter list
	for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
	{
		if ((*iter).col == collision && (*iter).isTrigger == true)
		{
			std::iter_swap(iter, enterCollisions.end() - 1);
			enterCollisions.pop_back();
			found = true;
			break;
		}
	}

	//Try to find in stay list
	if (!found)
	{
		for (auto iter = stayCollisions.begin(); iter != stayCollisions.end(); iter++)
		{
			if ((*iter).col == collision && (*iter).isTrigger == true)
			{
				std::iter_swap(iter, stayCollisions.end() - 1);
				stayCollisions.pop_back();
				found = true;
				break;
			}
		}
	}

	//Push to correct list
	if(found)
		exitCollisions.push_back(CollisionResolveInfo(collision, true));
	else enterCollisions.push_back(CollisionResolveInfo(collision, true));
}

// Add a collision to the resolver
void CollisionResolver::AddEnterCollision(Collision collision)
{
	enterCollisions.push_back(CollisionResolveInfo(collision, false));
}

// Exit a collision to the resolver. 
// Will remove an existing collision and add it to the exit list
void CollisionResolver::AddExitCollision(Collision collision)
{
	bool found = false;

	//Try to find in enter list
	for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
	{
		if ((*iter).col == collision && (*iter).isTrigger == false)
		{
			std::iter_swap(iter, enterCollisions.end() - 1);
			enterCollisions.pop_back();
			found = true;
			break;
		}
	}

	//Try to find in stay list
	if (!found)
	{
		for (auto iter = stayCollisions.begin(); iter != stayCollisions.end(); iter++)
		{
			if ((*iter).col == collision && (*iter).isTrigger == false)
			{
				std::iter_swap(iter, stayCollisions.end() - 1);
				stayCollisions.pop_back();
				found = true;
				break;
			}
		}
	}

	exitCollisions.push_back(CollisionResolveInfo(collision, false));
}

// Resolve all collision events for this resolver
// (OnCollisionEnter, OnCollisionStay, OnCollisionExit)
// (OnTriggerEnter, OnTriggerStay, OnTriggerExit)
void CollisionResolver::ResolveCollisions(const std::vector<UserComponent*>& ucs)
{
	//Don't run these if there's nothing to run
	if (ucs.size() > 0)
	{
		//Run all OnEnters
		for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
		{
			for each (UserComponent* uc in ucs)
			{
				if ((*iter).isTrigger)
					uc->OnTriggerEnter((*iter).col);
				else uc->OnCollisionEnter((*iter).col);
			}
		}
		//Run all OnStays
		for (auto iter = stayCollisions.begin(); iter != stayCollisions.end(); iter++)
		{
			for each (UserComponent* uc in ucs)
			{
				if ((*iter).isTrigger)
					uc->OnTriggerStay((*iter).col);
				else uc->OnCollisionStay((*iter).col);
			}
		}
	}

	//Run all OnExits
	if (exitCollisions.size() > 0)
	{
		//Don't run if there's nothing to run
		if (ucs.size() > 0)
		{
			for (auto iter = exitCollisions.begin(); iter != exitCollisions.end(); iter++)
			{
				for each (UserComponent* uc in ucs)
				{
					if ((*iter).isTrigger)
						uc->OnTriggerExit((*iter).col);
					else uc->OnCollisionExit((*iter).col);
				}
			}
		}
		//Still need to clear
		exitCollisions.clear();
	}

	//Place all elements in the stay list and clear the enter list
	if (enterCollisions.size() > 0)
	{
		for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
			stayCollisions.push_back(*iter);
		enterCollisions.clear();
	}
}

