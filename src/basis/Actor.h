#pragma once

#include "base/Math.hpp"
#include "base/Random.hpp"
#include "HelpFunctions.h"
#include <iostream>
#include <vector>

enum ActorType
{
	ActorType_Emitter,
	ActorType_Particle,
	ActorType_Deflecter,
	ActorType_ParticleEmitter
};

class System;
class Renderer;

class StateEval
{
public:
	StateEval() {}
	StateEval(const FW::Vec3f& dx, const FW::Vec3f& dv) :
		dx(dx),
		dv(dv)
	{}
	~StateEval() {}

	FW::Vec3f dx;
	FW::Vec3f dv;

};

class State
{
public:
	State() {}
	State(const FW::Vec3f& p, const FW::Vec3f& v) : 
		pos(p),
		vel(v)
	{}
	~State() {}
	FW::Vec3f pos;
	FW::Vec3f vel;

	void updateState(const StateEval& s, const float f = 1.0f) { pos += s.dx * f; vel += s.dv * f; }
	void print() { std::cout << "State: Pos - " << pos.x << "/" << pos.y << "/" << pos.z << " Vel - " << vel.x << "/" << vel.y<< "/" << vel.z << std::endl; }
};

class Actor
{
public:
	Actor(ActorType type, const float lifetime) :
	m_type(type),
	m_lifetime(lifetime),
	m_timer(0.0f),
	m_state(State())
	{}
	virtual ~Actor() {}

	virtual ActorType getActorType() const { return m_type; }
	virtual StateEval evalF(const float, const State&,std::vector<Actor*>&) { std::cout << "Used default for evalF.." << std::endl; return StateEval(); }
	virtual State getState() const { return m_state; }
	virtual FW::Vec3f getPos() const { return m_state.pos; }
	
	virtual void setState(const State& s) { m_state.pos = s.pos; m_state.vel = s.vel; }
	virtual void setTime(const float dt) { m_timer += dt; }

	virtual bool isDone() const { return m_timer > m_lifetime; }
	virtual void draw(Renderer&) {}

protected:
	State		m_state;
	ActorType	m_type;
	float		m_lifetime;
	float		m_timer;
};

template<typename System>
class Particle : public Actor
{
public:
	Particle(const float mass, const FW::Vec3f& p, const FW::Vec3f& v, const float lifetime, ActorType type = ActorType_Particle) :
		m_mass(mass),
		m_invertMass(1.0f/mass),
		Actor(type, lifetime)
	{
		m_state = State(p, v);
	}

	virtual ~Particle() {}

	float getMass() const { return m_mass; }
	float getDuration() const { return m_duration; }
	float getTimeElapset() const {return m_timeElapsed; }
	float getInverseMass()  const { return m_invertMass; }

	void setAcc(const FW::Vec3f a) { m_state.acc = a; }
	void addToTimer(const float d) { m_timer += d; }
	
	virtual StateEval evalF(const float, const State&,std::vector<Actor*>&);
	virtual void draw(Renderer& renderer);

protected:
	float m_mass;
	float m_invertMass;
};

template<typename System>
class TraingleEmitter : public Actor
{
public:
	TraingleEmitter(const FW::Vec3f& posa, const FW::Vec3f& posb, const FW::Vec3f& posc, const float minSpeed,
		const float maxSpeed, const float timeBetweenParticles, const float particleMass, const float lifetimeOfParticles) :
		m_posA(posa),
		m_posB(posb),
		m_posC(posc),
		m_minSpeed(minSpeed),
		m_maxSpeed(maxSpeed),
		m_timeBetween(timeBetweenParticles),
		m_particleMass(particleMass),
		m_lifetimeOfParticles(lifetimeOfParticles),
		m_previousTick(0.0f),
		Actor(ActorType_Emitter, 1.0f)
	{
		m_normal = FW::cross((m_posB - m_posA), (m_posC - m_posA));
		m_normal = m_normal.normalized();
		m_formBasis = formBasis(m_normal);
		m_randomGen = FW::Random();
		m_state.pos = (m_posA + m_posB + m_posC)/3.0f; 
	}
	
	virtual ~TraingleEmitter() {}

	virtual StateEval evalF(const float, const State&,std::vector<Actor*>&);
	virtual bool isDone() const { return false; }

private:
	FW::Random m_randomGen;
	FW::Mat3f m_formBasis;
	FW::Vec3f m_posA;
	FW::Vec3f m_posB;
	FW::Vec3f m_posC;
	FW::Vec3f m_normal;
	float m_timeBetween;
	float m_previousTick;
	float m_minSpeed;
	float m_maxSpeed;
	float m_particleMass;
	float m_lifetimeOfParticles;
};

template<typename System>
class Deflecter : public Actor
{
public:
	Deflecter(const FW::Vec3f pos, const float range, const float k) :
		m_position(pos),
		Actor(ActorType_Deflecter)
	{}
	virtual ~Deflecter() {}

	virtual StateEval evalF(const float, const State&,std::vector<Actor*>&);

private:
	float m_range;
	float m_k;
};

template<typename System>
class ParticleEmitter : public Actor
{
public:
	ParticleEmitter(const float mass, const FW::Vec3f& p, const FW::Vec3f& v, const float lifetime,
		const float lifetimeOfParticles, const float timeBetween,
		const float minSpeed, const float maxSpeed, ActorType type = ActorType_ParticleEmitter) :
			m_timeBetween(timeBetween),
			m_previousTick(0.0f),
			m_minSpeed(minSpeed),
			m_maxSpeed(maxSpeed),
			m_lifetimeOfParticles(lifetimeOfParticles),
			m_pacticleMass(mass),
			Actor(ActorType_ParticleEmitter, lifetime)
	{
			m_randomGen = FW::Random();
			m_carrier = new Particle<System>(mass, p, v, lifetime);
			m_state = m_carrier->getState();
	}
	virtual ~ParticleEmitter() 
	{
		delete m_carrier;
	}

	virtual void draw(Renderer&);
	virtual StateEval evalF(const float, const State&,std::vector<Actor*>&);

private:
	FW::Random m_randomGen;
	Particle<System>* m_carrier;
	float m_timeBetween;
	float m_previousTick;
	float m_minSpeed;
	float m_maxSpeed;
	float m_pacticleMass;
	float m_lifetimeOfParticles;
};

template<typename System>
void Particle<System>::draw(Renderer& renderer) 
{
	renderer.drawTriangleToCamera(m_state.pos, FW::Vec4f(1.0f, 1.0f, 0.0f, 1.0f));
}

template<typename System>
void ParticleEmitter<System>::draw(Renderer& renderer)
{
	renderer.drawTriangleToCamera(m_carrier->getPos(), FW::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
}