#pragma once

#include "base/Random.hpp"
#include <vector>

class Actor;

class System
{
public:
	System() 
	{
		m_createBuffer = new Actor*[1028];
		m_deleteBuffer = new int[1028];

	}
	virtual ~System();

	virtual void evalSystem(const float);
	virtual void draw(const float);

protected:
	std::vector<Actor*> m_actors;
	Actor** m_createBuffer;
	int* m_deleteBuffer;

};

class ParticleSystem : public System
{
public:
	ParticleSystem();
	virtual ~ParticleSystem() {}
};

class BoidSystem : public System
{
public:
	BoidSystem(const float, const size_t);
	virtual ~BoidSystem() {}
	virtual void evalSystem(const float);

private:
	std::vector<Actor*> m_closeBuffer;
	float m_closeDistance;
	size_t m_numberOfParticles;
};