#include "TestCallbacks.h"
#include "Collider.h"

TestCallbacks::TestCallbacks(GameObject* gameObject) : UserComponent(gameObject)
{
}


TestCallbacks::~TestCallbacks()
{
}

void TestCallbacks::OnCollisionEnter(Collision collision)
{
	//printf("%s entered %s\n", collision.gameObject->GetName().c_str(), gameObject()->GetName().c_str());
}

void TestCallbacks::OnCollisionStay(Collision collision)
{
	//printf("%s stayed on %s\n", collision.gameObject->GetName().c_str(), gameObject()->GetName().c_str());
}

void TestCallbacks::OnCollisionExit(Collision collision)
{
	//printf("%s exited %s\n", collision.gameObject->GetName().c_str(), gameObject()->GetName().c_str());
}

void TestCallbacks::OnTriggerEnter(Collision collision)
{
	printf("%s trigger entered %s\n", collision.gameObject->GetName().c_str(), gameObject()->GetName().c_str());
}

void TestCallbacks::OnTriggerStay(Collision collision)
{
	printf("%s trigger stayed on %s\n", collision.gameObject->GetName().c_str(), gameObject()->GetName().c_str());
}

void TestCallbacks::OnTriggerExit(Collision collision)
{
	printf("%s trigger exited %s\n", collision.gameObject->GetName().c_str(), gameObject()->GetName().c_str());
}