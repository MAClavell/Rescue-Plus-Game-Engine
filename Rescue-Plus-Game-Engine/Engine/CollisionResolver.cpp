#include "CollisionResolver.h"
#include "GameObject.h"

using namespace std;

// Internal function for sending collisions
void CollisionResolver::SendCollision(CollisionResolveInfo resolveInfo)
{
	bool found = false;

	//Try to find in enter list
	for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
	{
		if ((*iter) == resolveInfo)
		{
			iter_swap(iter, enterCollisions.end() - 1);
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
			if ((*iter) == resolveInfo)
			{
				iter_swap(iter, stayCollisions.end() - 1);
				stayCollisions.pop_back();
				found = true;
				break;
			}
		}
	}

	//Push to correct list
	if(found)
		exitCollisions.push_back(resolveInfo);
	else enterCollisions.push_back(resolveInfo);
}

// Send a trigger to the resolver and have it decide what events to run.
// Since PhysX doesn't have PxTriggerPairFlags for enter and exit,
// we have to manually check if a collision is entering or exiting
void CollisionResolver::SendTriggerCollision(Collision collision)
{
	SendCollision(CollisionResolveInfo(collision, true));
}

// Send a collision to the resolver and have it decide what events to run.
// Since PhysX doesn't have flags for enter and exit when using PxController,
// we have to manually check if a collision is entering or exiting.
//
// Use AddEnter and AddExit if you can
void CollisionResolver::SendCollision(Collision collision)
{
	SendCollision(CollisionResolveInfo(collision, false));
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
			iter_swap(iter, enterCollisions.end() - 1);
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
				iter_swap(iter, stayCollisions.end() - 1);
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
void CollisionResolver::ResolveCollisions(GameObject* obj)
{
	vector<UserComponent*> colEnt, colSty, colExt, trigEnt, trigSty, trigExt;
	obj->GetCollisionAndTriggerCallbackComponents(&colEnt, &colSty, &colExt,
		&trigEnt, &trigSty, &trigExt);

	if (colEnt.size() > 0 || trigEnt.size() > 0)
	{
		//Run all OnEnters
		for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
		{
			if ((*iter).isTrigger)
				for each(UserComponent* uc in trigEnt)
					uc->OnTriggerEnter((*iter).col);
			else
				for each (UserComponent* uc in colEnt)
					uc->OnCollisionEnter((*iter).col);
		}
	}
	
	if (colSty.size() > 0 || trigSty.size() > 0)
	{
		//Run all OnStays
		for (auto iter = stayCollisions.begin(); iter != stayCollisions.end(); iter++)
		{
			if ((*iter).isTrigger)
				for each(UserComponent* uc in trigSty)
					uc->OnTriggerStay((*iter).col);
			else
				for each (UserComponent* uc in colSty)
					uc->OnCollisionStay((*iter).col);
		}
	}

	//Run all OnExits
	if (exitCollisions.size() > 0)
	{
		if (colExt.size() > 0 || trigExt.size() > 0)
		{
			for (auto iter = exitCollisions.begin(); iter != exitCollisions.end(); iter++)
			{
				if ((*iter).isTrigger)
					for each(UserComponent* uc in trigExt)
						uc->OnTriggerExit((*iter).col);
				else
					for each (UserComponent* uc in colExt)
						uc->OnCollisionExit((*iter).col);
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

