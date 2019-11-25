#pragma once
#include <vector>
#include <functional>

// --------------------------------------------------------
// A class for holding and sending messages
// --------------------------------------------------------
template<typename... Args>
class Messenger
{
private:
	std::vector<std::function<void(Args... args)>> actions;

public:

	// --------------------------------------------------------
	// Add a listiner to this messenger
	// --------------------------------------------------------
	void AddListener(std::function<void(Args... args)> function)
	{
		actions.push_back(function);
	}

	// --------------------------------------------------------
	// Remove a listener from this messenger
	// --------------------------------------------------------
	//TODO: Add removal
	void RemoveListener(std::function<void(Args... args)> function)
	{
		//for (auto iter = actions.begin(); iter != actions.end(); iter++)
		//{
		//	if (*iter == function)
		//	{
		//		std::iter_swap(iter, actions.end() - 1);
		//		actions.pop_back();
		//		break;
		//	}
		//}
	}

	// --------------------------------------------------------
	// Invoke this messenger
	// --------------------------------------------------------
	void Invoke(Args... args)
	{
		for (auto iter = actions.begin(); iter != actions.end(); iter++)
		{
			(*iter)(args...);
		}
	}
};