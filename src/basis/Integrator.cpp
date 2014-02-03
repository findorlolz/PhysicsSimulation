#include "Integrator.h"
#include "Actor.h"


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
	State current = actor->getState();
	StateEval tmp = actor->evalF(dt, current, actorlist);
	current.updateState(tmp);
	actor->setTime(dt);
	actor->setState(current);
}

Runge_KuttaIntegrator& Runge_KuttaIntegrator::get()
{
	static Runge_KuttaIntegrator* gpSingleton = nullptr;
	if (gpSingleton == nullptr)
	{
			gpSingleton = new Runge_KuttaIntegrator();
	}
	FW_ASSERT(gpSingleton != nullptr && "Failed to create Runge_KuttaIntegrator");
	return *gpSingleton;
}

void Runge_KuttaIntegrator::evalIntegrator(const float dt, Actor* actor, std::vector<Actor*>& actors)
{
	State current = actor->getState();
	StateEval tmp;
	const float f = 1.0f/6.0f;

	State k1 = current;
	State k2 = current;
	State k4 = current;
	State k3 = current;

	tmp = actor->evalF(dt, current, actors);
	k1.updateState(tmp);
	current.updateState(tmp, f);
	tmp = actor->evalF(dt*0.5f, k1, actors);
	k2.updateState(tmp);
	current.updateState(tmp,2.0*f);
	tmp = actor->evalF(dt*0.5f, k2, actors);
	k3.updateState(tmp);
	current.updateState(tmp,2.0*f);
	tmp = actor->evalF(dt, k3, actors);
	k4.updateState(tmp);
	current.updateState(tmp, f);
	actor->setState(current);
	actor->setTime(dt);
}