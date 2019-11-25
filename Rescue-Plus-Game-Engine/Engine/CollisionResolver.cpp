#include "CollisionResolver.h"
#include "Component.h"

void CollisionResolver::AddEnterCollision(Collision collision)
{
	enterCollisions.push_back(CollisionResolveInfo(collision));
}

void CollisionResolver::AddExitCollision(Collision collision)
{
	bool found = false;

	//Try to find in enter list
	for (auto iter = enterCollisions.begin(); iter != enterCollisions.end(); iter++)
	{
		if ((*iter).col == collision)
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
			if ((*iter).col == collision)
			{
				std::iter_swap(iter, stayCollisions.end() - 1);
				stayCollisions.pop_back();
				found = true;
				break;
			}
		}
	}

	exitCollisions.push_back(CollisionResolveInfo(collision));
}

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
				uc->OnCollisionEnter((*iter).col);
			}
		}
		//Run all OnStays
		for (auto iter = stayCollisions.begin(); iter != stayCollisions.end(); iter++)
		{
			for each (UserComponent* uc in ucs)
			{
				uc->OnCollisionStay((*iter).col);
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
					uc->OnCollisionExit((*iter).col);
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

