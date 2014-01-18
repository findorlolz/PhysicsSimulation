#pragma once

#include <vector>
#include "base/Math.hpp"

#include "Actor.h"

class EulerIntegrator
{
public:
	EulerIntegrator() {}
    ~EulerIntegrator() {}

	static EulerIntegrator& get();
	void evalIntegrator(const float dt, Actor*, std::vector<Actor*>&);
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
