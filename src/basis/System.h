#pragma once

#include "Integrator.h"
#include "Renderer.h"
#include "base/Random.hpp"

class Renderer;

class Particle
{
public:
	Particle(const float mass, const FW::Vec3f p, const FW::Vec3f v) :
		m_mass(mass),
		m_invertMass(1.0f/mass),
		m_position(p),
		m_velocity(v),
		m_acceleration(0.0f),
		m_duration(0.0f)
	{}
	~Particle() {}

	FW::Vec3f getAcc() const { return m_acceleration; }
	FW::Vec3f getVel() const { return m_velocity; }
	FW::Vec3f getPos() const { return m_position; }

	void setState(const State& s) { m_position = s.m_pos; m_velocity = s.m_vec; m_acceleration = s.m_acc; }

private:
	FW::Vec3f m_acceleration;
	FW::Vec3f m_velocity;
	FW::Vec3f m_position;
	float m_mass;
	float m_invertMass;
	float m_duration;
};

class Emitter
{
public:
	Emitter(const FW::Vec3f& posa, const FW::Vec3f& posb, const FW::Vec3f& posc, const float minSpeed,
		const float maxSpeed, const float timeBetweenParticles, const float particleMass) :
		m_posA(posa),
		m_posB(posb),
		m_posC(posc),
		m_minSpeed(minSpeed),
		m_maxSpeed(maxSpeed),
		m_timeBetween(timeBetweenParticles),
		m_particleMass(particleMass),
		m_previousTick(0.0f)
	{
		m_normal = FW::cross((m_posB - m_posA), (m_posC - m_posA));
		m_normal = m_normal.normalized();
		m_randomGen = FW::Random();
	}
	~Emitter() 
	{
		for(auto i : m_particles)
			delete i;
	}

	std::vector<Particle*> getParticles() const { return m_particles; }
	void evalF(const float);
	void draw(const float);

private:
	std::vector<Particle*> m_particles;
	FW::Random m_randomGen;
	FW::Vec3f m_posA;
	FW::Vec3f m_posB;
	FW::Vec3f m_posC;
	FW::Vec3f m_normal;
	float m_timeBetween;
	float m_previousTick;
	float m_minSpeed;
	float m_maxSpeed;
	float m_particleMass;


};

class System
{
public:
	System() {}
	virtual ~System() {}

	virtual State evalState(const float dt) {}
	virtual void draw(const float) {}
	virtual State evalFunc(const State&, const float dt) {}
};

class ParticleSystem : public System
{
public:
	ParticleSystem() : 
		System(),
		m_emitter(Emitter(FW::Vec3f(1,0,0),FW::Vec3f(0,0,0), FW::Vec3f(0,0,1), 0.1f, 10.0f, 0.5f, 1.0f))
	{}
	virtual ~ParticleSystem() {}

	virtual State evalState(const float dt) { m_emitter.evalF(dt); }
	virtual void draw(const float scale) { m_emitter.draw(scale); }
	State evalFunc(const State&, const float dt);

private:

	Emitter m_emitter;
};