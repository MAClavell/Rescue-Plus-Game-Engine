#pragma once
#include <vector>
#include <functional>

template<typename... Args>
class Messenger
{
private:
	std::vector<std::function<void(Args...)>> actions;

public:
	bool HasActions()
	{
		return actions.size() > 0;
	}

	void AddListener(std::function<void(Args...)> function)
	{
		actions.push_back(function);
	}

	void Invoke(Args... args)
	{
		for each (auto action in actions)
		{
			action(args...);
		}
	}
};