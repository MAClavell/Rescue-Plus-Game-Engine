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
	for (size_t i = enterCollisions.size() - 1; i > 0; i--)
	{
		auto iter = enterCollisions.begin() + i;
		if ((*iter).col == collision && enterCollisions.size() > 0)
		{
			std::iter_swap(iter, enterCollisions.end() - 1);
			enterCollisions.pop_back();
			found = true;
			break;
		}
	}

	//Try to find in stay list
	if (!found && stayCollisions.size() > 0)
	{
		for (size_t i = stayCollisions.size() - 1; i > 0; i--)
		{
			auto iter = stayCollisions.begin() + i;
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
	if (ucs.size() > 0)
	{
		//Run all OnEnters
		if (enterCollisions.size() > 0)
		{
			for (auto iter = enterCollisions.end() - 1; iter != enterCollisions.begin(); iter--)
			{
				CollisionResolveInfo colInfo = *iter;
				for each (UserComponent* uc in ucs)
				{
					uc->OnCollisionEnter((*iter).col);
				}
			}
		}
		//Run all OnStays
		if (stayCollisions.size() > 0)
		{
			for (auto iter = stayCollisions.end() - 1; iter != stayCollisions.begin(); iter--)
			{
				CollisionResolveInfo colInfo = *iter;
				for each (UserComponent* uc in ucs)
				{
					uc->OnCollisionStay((*iter).col);
				}
			}
		}
	}

	//Run all OnExits
	if (exitCollisions.size() > 0)
	{
		for (size_t i = exitCollisions.size() - 1; i > 0; i--)
		{
			auto iter = exitCollisions.begin() + i;
			if (ucs.size() > 0)
			{
				for each (UserComponent* uc in ucs)
				{
					uc->OnCollisionExit((*iter).col);
				}
			}

			//Remove from exit list
			std::iter_swap(iter, exitCollisions.end() - 1);
			exitCollisions.pop_back();
		}
	}

	//Remove from enter list and place in stay list
	if (enterCollisions.size() > 0)
	{
		for (size_t i = enterCollisions.size() - 1; i > 0; i--)
		{
			auto iter = enterCollisions.begin() + i;
			CollisionResolveInfo colInfo = *iter;
			std::iter_swap(iter, enterCollisions.end() - 1);
			enterCollisions.pop_back();
			stayCollisions.push_back(colInfo);
		}
	}
}

