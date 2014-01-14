#pragma once

#include <vector>
#include "base/Math.hpp"


struct State
{
	State(FW::Vec3f p, FW::Vec3f v, FW::Vec3f a) :
		m_pos(p),
		m_vec(v),
		m_acc(a)
	{}

	FW::Vec3f m_pos;
	FW::Vec3f m_vec;
	FW::Vec3f m_acc;
};

class System;
class ParticleSystem;

/*
State operator+(const State& a, const State& b)
{
	return State((a.m_pos + b.m_pos), (a.m_vec + b.m_pos), (a.m_acc + b.m_acc));
}

State operator*(const float d, const State s)
{
	return State(d*s.m_pos, d*s.m_vec, d*s.m_acc);
}
*/

class EulerIntegrator
{
public:
	EulerIntegrator() {}
    ~EulerIntegrator() {}

	static EulerIntegrator& get()
    {
		static EulerIntegrator* gpSingleton = nullptr;
		if (gpSingleton == nullptr)
		{
				gpSingleton = new EulerIntegrator();
		}
		FW_ASSERT(gpSingleton != nullptr && "Failed to create EulerIntegrator");
		return *gpSingleton;
    }

	State eval(const float dt, const State current, System* system)
    {
		return system->evalFunc(current, dt);
    }
};
/*
class Krunge_KuttaIntegrator: public Integrator {
public:
    Krunge_KuttaIntegrator(float h, ParticleSystem* pset): Integrator(h, pset) {}

    ~Krunge_KuttaIntegrator() {}

    vector<Particle>& stepSystem()
    {
        vector<osg::Vec3f> currentState;
        vector<osg::Vec3f> finalState;

        vector<osg::Vec3f> k1;
        vector<osg::Vec3f> k2;
        vector<osg::Vec3f> k3;
        vector<osg::Vec3f> k4;

        currentState = ps->getState();

        //1st step
        k1 = ps->evalF(currentState);

        //2nd step
        for (size_t i = 0; i < currentState.size(); i++)
        {
            k2.push_back(currentState[i]  + k1[i] * 0.5f * h);
        }
        k2 = ps->evalF(k2);

        //3rd step
        for (size_t i = 0; i < currentState.size(); i++)
        {
            k3.push_back(currentState[i]  + k2[i] * 0.5f * h);
        }
        k3 = ps->evalF(k3);

        //4th step
        for (size_t i = 0; i < currentState.size(); i++)
        {
            k4.push_back(currentState[i]  + k3[i] * h);
        }
        k4 = ps->evalF(k4);

        //getting final state
        for (size_t i = 0; i < currentState.size(); i++)
        {
            finalState.push_back(currentState[i]  + ( k1[i] + k2[i] * 2.0f + k3[i] * 2.0f + k4[i]) * (h/6.0f));
        }
        ps->setState(finalState);
        return ps->updateParticeles();
    }
};
*/
