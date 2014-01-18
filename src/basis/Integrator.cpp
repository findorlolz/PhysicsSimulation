#include "Integrator.h"
#include "System.h"


EulerIntegrator& EulerIntegrator::get()
{
	static EulerIntegrator* gpSingleton = nullptr;
	if (gpSingleton == nullptr)
	{
			gpSingleton = new EulerIntegrator();
	}
	FW_ASSERT(gpSingleton != nullptr && "Failed to create EulerIntegrator");
	return *gpSingleton;
}

void EulerIntegrator::evalIntegrator(const float dt, Actor* actor, std::vector<Actor*>& actorlist)
{
	actor->evalF(dt, actorlist);
}