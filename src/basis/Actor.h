#pragma once

#include "base/Math.hpp"
#include "base/Random.hpp"
#include <iostream>
#include <vector>
#include "HelpFunctions.h"

enum ActorType
{
	ActorType_Emitter,
	ActorType_Particle,
	ActorType_SquareEmitter,
	ActorType_Vortex,
	ActorType_ParticleEmitter
};

class System;
class Renderer;
class ActorContainer;


class StateEval
{
public:
	StateEval(){}
	StateEval(const FW::Vec3f& dx, const FW::Vec3f& dv) :
		dx(dx),
		dv(dv)
	{
	}

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

	State& operator=(const State&) { return *this; }

	void updateState(const StateEval& s, const float f = 1.0f) { pos += s.dx * f; vel += s.dv * f; }
	void print() { std::cout << "State: Pos - " << pos.x << "/" << pos.y << "/" << pos.z << " Vel - " << vel.x << "/" << vel.y<< "/" << vel.z << std::endl; }
};

const State zeroState = State(FW::Vec3f(), FW::Vec3f());

class Actor
{
public:
	Actor(ActorType type, const float lifetime, float influenceDistance = .0f) :
		m_influenceDistance(influenceDistance),
		m_type(type),
		m_lifetime(lifetime),
		m_timer(0.0f)
	{
	}
	virtual ~Actor() {}

	virtual ActorType getActorType() const { return m_type; }
	virtual StateEval evalF(const float, const State&, ActorContainer& ) { std::cout << "Used default for evalF.." << std::endl; return StateEval(); }
	virtual State getState() const { return m_state; }
	virtual FW::Vec3f getPos() const { return m_state.pos; }
	virtual float getInfluenceDistance() const { return m_influenceDistance; }
	
	virtual void setStateBuffer(const State&s) { m_stateBuffer.pos = s.pos; m_stateBuffer.vel = s.vel; }
	virtual void addToStateBuffer(const State&s) { m_stateBuffer.pos += s.pos; m_stateBuffer.vel += s.vel; }
	virtual void addToBufferPosition(const FW::Vec3f& v) { m_stateBuffer.pos += v; };
	virtual void updateStateFromBuffer() { setState(m_stateBuffer); }
	virtual void resetStateBuffer() { setStateBuffer(zeroState); }
	
	virtual void setTime(const float dt) { m_timer += dt; }
	virtual void setState(const State& s) { m_state.pos = s.pos; m_state.vel = s.vel; }
	virtual bool isDone() const { return m_timer > m_lifetime; }
	virtual bool hasInfluence() const { return m_influenceDistance > .001f; }
	virtual void draw(Renderer&) {}

	virtual float getTimer() const { return m_timer; }
	virtual float getLifetime() const { return m_lifetime; }
	virtual FW::Vec3f getVortexStr() const { return FW::Vec3f(); }
	virtual void activeVortexVisible()  { m_hasVortexEffect = true; }
	virtual void resetVortexVisible() { m_hasVortexEffect = false; }

protected:

	State		m_state;
	State		m_stateBuffer;
	ActorType	m_type;
	float		m_lifetime;
	float		m_timer;
	float		m_influenceDistance;
	bool		m_hasVortexEffect;
};

template<typename System>
class Particle : public Actor
{
public:
	Particle(const float mass, const FW::Vec3f& p, const FW::Vec3f& v, const float lifetime, bool vortexEnabled = false, ActorType type = ActorType_Particle) :
		m_mass(mass),
		m_invertMass(1.0f/mass),
		Actor(type, lifetime)
	{
		setState(State(p, v));
		setStateBuffer(m_state);
		if(vortexEnabled)
			m_vortexStr = FW::Random().getF32(.0f, 1.f);
	}

	virtual ~Particle() {}

	float getMass() const { return m_mass; }
	float getDuration() const { return m_duration; }
	float getTimeElapset() const {return m_timeElapsed; }
	float getInverseMass()  const { return m_invertMass; }

	void setAcc(const FW::Vec3f a) { m_state.acc = a; }
	void addToTimer(const float d) { m_timer += d; }
	
	virtual StateEval evalF(const float, const State&,ActorContainer&);
	virtual void draw(Renderer& renderer);
	virtual float getVortexStr() { return m_vortexStr; }

protected:
	float m_vortexStr;
	float m_mass;
	float m_invertMass;
};

const float rad_squared = .01f;

template<typename System>
class Vortex : public Actor
{
public:
	Vortex(const FW::Vec3f& p, const FW::Vec3f& v, const bool vortexDir, const float lifetime, const float influenceDistance, const FW::Vec3f& vortexStr, ActorType type = ActorType_Vortex) :
		Actor(type, lifetime, influenceDistance),
		m_vortexDir(vortexDir)
	{
		m_carrier = new Particle<System>(1.0f, p, v, lifetime);
		m_state = m_carrier->getState();
		setStateBuffer(m_state);
	}

	virtual ~Vortex() {}
	virtual FW::Vec3f getPos() const { return m_carrier->getPos(); }

	virtual StateEval evalF(const float dt, const State& current, ActorContainer& c)
	{
		float invSize = 1.0f/((float) c.createdActors.size());
		FW::Vec3f pos = m_carrier->getPos();
		for(auto i : c.createdActors)
		{
			i->activeVortexVisible();
			FW::Vec3f vel;
			float distToVortex = (pos-i->getPos()).length();
			float invCubeDistToVortex = 1.0f/(distToVortex*distToVortex*distToVortex);
			float currentLenght = m_carrier->getState().vel.length();
			for(auto j : c.createdActors)
			{
				if(i != j)
				{
					FW::Vec3f str = j->getVortexStr();
					FW::Vec3f d = i->getPos() - j->getPos();
					float dist_squared = d.x*d.x+d.y*d.y+d.z*d.z;
					dist_squared += rad_squared;
					float dist = FW::sqrt(dist_squared);
					float factor = 1.0f/(dist_squared*dist);
					d *= factor;
					if(m_vortexDir)
						vel += FW::Vec3f((d.z*str.y - d.y*str.z),(d.x*str.z-d.z*str.x),(d.y*str.x-d.x*str.y));
					else
						vel -= FW::Vec3f((d.z*str.y - d.y*str.z),(d.x*str.z-d.z*str.x),(d.y*str.x-d.x*str.y));
				}
			}
			FW::Vec3f eval = dt*(vel.normalized()*(.01f * currentLenght*invSize*invCubeDistToVortex));
			i->addToBufferPosition(eval*((FlowControl*)c.data2)->getFlowSpeed());
		}

		Runge_KuttaIntegrator integrator = Runge_KuttaIntegrator::get();
		integrator.evalIntegrator(dt, m_carrier, c);
		m_carrier->updateStateFromBuffer();
		setState(m_carrier->getState());
		return StateEval();
	}
	virtual void draw(Renderer& renderer);

private:
	FW::Vec3f m_vortexStrVector;
	Particle<System>* m_carrier;
	bool m_vortexDir;
};


class PlaneEmitter : public Actor
{
public:
	PlaneEmitter(const FW::Vec3f& posa, const FW::Vec3f& posb, const FW::Vec3f& posc, const float minSpeed,
		const float maxSpeed, const float timeBetweenParticles, const float lifetimeOfParticles, const float particleMass,
		bool useParticleEmitters) :
		m_posA(posa),
		m_posB(posb),
		m_posC(posc),
		m_minSpeed(minSpeed),
		m_maxSpeed(maxSpeed),
		m_timeBetween(timeBetweenParticles),
		m_particleMass(particleMass),
		m_lifetimeOfParticles(lifetimeOfParticles),
		m_previousTick(0.0f),
		m_particleEmitters(useParticleEmitters),
		Actor(ActorType_Emitter, 1.0f)
	{
		m_normal = FW::cross((m_posB - m_posA), (m_posC - m_posA));
		m_normal = m_normal.normalized();
		m_formBasis = formBasis(m_normal);
		m_randomGen = FW::Random();
		m_state.pos = (m_posA + m_posB + m_posC)/3.0f; 
	}
	
	virtual ~PlaneEmitter() {}

	virtual StateEval evalF(const float, const State&, ActorContainer&) { return StateEval(); }
	virtual bool isDone() const { return false; }

protected:
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
	bool m_particleEmitters;
};

template<typename System>
class SquareEmitter : public PlaneEmitter
{
public:
	SquareEmitter(const FW::Vec3f& posA, const FW::Vec3f& posB, const FW::Vec3f& posC,
		const float timeBetweenParticles, const float particleLifetime, const float particleMass, bool useParticleEmitters) :
	PlaneEmitter(posA, posB, posC, 0.0f, 1.0f, timeBetweenParticles, particleLifetime, particleMass, useParticleEmitters)
	{
	}
	virtual ~SquareEmitter() {}

	StateEval evalF(const float dt, const State&, ActorContainer& c)
	{	
		float tmp = dt + m_previousTick;
		StateEval state = StateEval();
		while(tmp >= m_timeBetween)
		{
			float r1 = m_randomGen.getF32(0,1.0f);
			float r2 = m_randomGen.getF32(0,1.0f);
			FW::Vec3f p = m_posA + r1*(m_posB - m_posA) + r2*(m_posC - m_posA);
			if(m_particleEmitters)
			{
				ParticleEmitter<System>* particle = new (((MemPool*) c.data2)->alloc()) ParticleEmitter<System>(1.0f,p,FW::Vec3f(1.0f, 0.0f, 0.0f),m_lifetimeOfParticles, m_lifetimeOfParticles,
					m_timeBetween, m_minSpeed, m_maxSpeed);
				c.createdActors.push_back(particle);
			}
			else
			{
				Particle<System>* particle = new (((MemPool*) c.data1)->alloc()) Particle<System>(1.0f,p,FW::Vec3f(1.0f, 0.0f, 0.0f),m_lifetimeOfParticles, true);			
				c.createdActors.push_back(particle);
			}
			tmp -= m_timeBetween;
		}
		m_previousTick = tmp;
		return state;
	}
};

template<typename System>
class TriangleEmitter : public PlaneEmitter
{
public:
	TriangleEmitter(const FW::Vec3f& posA, const FW::Vec3f& posB, const FW::Vec3f& posC, const float minSpeed, const float maxSpeed,
		const float timeBetweenParticles, const float particleLifetime, const float particleMass, bool useParticleEmitters) :
	PlaneEmitter(posA, posB, posC, minSpeed, maxSpeed, timeBetweenParticles, particleLifetime, particleMass, useParticleEmitters)
	{
	}
	virtual ~TriangleEmitter() {}

	StateEval evalF(const float dt, const State&, ActorContainer& c)
	{	
		float tmp = dt + m_previousTick;
		StateEval state = StateEval();
		while(tmp >= m_timeBetween)
		{
			float sqr_r1 = FW::sqrt(m_randomGen.getF32(0,1.0f));
			float r2 = m_randomGen.getF32(0,1.0f);
			FW::Vec3f p = (1-sqr_r1)*m_posA + sqr_r1*(1-r2)*m_posB + sqr_r1*r2*m_posC;
			FW::Vec3f v = FW::Vec3f(1.0f, 0.0f, 0.0f);
			if(m_particleEmitters)
			{
				ParticleEmitter<System>* particle = new (((MemPool*) c.data2)->alloc()) ParticleEmitter<System>(1.0f,p,v,m_lifetimeOfParticles, m_lifetimeOfParticles,
					m_timeBetween, m_minSpeed, m_maxSpeed);
				c.createdActors.push_back(particle);
			}
			else
			{
				Particle<System>* particle = new (((MemPool*) c.data1)->alloc()) Particle<System>(1.0f,p,v,m_lifetimeOfParticles);
				c.createdActors.push_back(particle);
			}
			tmp -= m_timeBetween;
		}
		m_previousTick = tmp;
		return state;
}
};

template<typename System>
class SquareVortexEmitter : public PlaneEmitter
{
public:
	SquareVortexEmitter(const FW::Vec3f& posA, const FW::Vec3f& posB, const FW::Vec3f& posC,
		const float timeBetweenParticles, const float particleLifetime, const float startDistance) :
	PlaneEmitter(posA, posB, posC, 0.0f, 1.0f, timeBetweenParticles, particleLifetime, 0.0f, false),
		m_startDistance(startDistance),
		m_stepper(1.f)
	{
	}
	virtual ~SquareVortexEmitter() {}

	virtual StateEval evalF(const float dt, const State&, ActorContainer& c)
	{	
		float tmp = dt + m_previousTick;
		StateEval state = StateEval();
		while(tmp >= m_timeBetween)
		{
			float r3 = m_randomGen.getF32(0,1.0f);
			if(r3 < .5f)
			{
				FW::Vec2f r1 = m_randomGen.getVec2f(0,1.0f);
				FW::Vec3f p = m_posA + r1.x*(m_posB - m_posA) + r1.y*(m_posC - m_posA);
				float r4 = m_randomGen.getF32(.0f, 1.0f);
				bool dir = true;
				if(r4 > .5)
					dir = false;
				FW::Vec3f str = m_randomGen.getVec3f(.0f, 1.0f);
				Actor* a = new Vortex<System>(p, FW::Vec3f(1.0f, .0f, .0f), dir, m_lifetimeOfParticles, m_stepper * m_startDistance, str * m_stepper);
				c.createdActors.push_back(a);
				m_stepper = 1.f;
			}
			else
			{
				m_stepper *= 2.f;
			}
			tmp -= m_timeBetween;
		}
		m_previousTick = tmp;
		return state;
	}

private:
	float m_change;
	float m_stepSize;
	float m_stepper;
	float m_startDistance;
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
			setStateBuffer(m_state);
	}
	virtual ~ParticleEmitter() 
	{
		delete m_carrier;
	}

	virtual void draw(Renderer&);
	StateEval evalF(const float dt, const State&, ActorContainer& c)
	{
		float tmp = dt + m_previousTick;
		while(tmp >= m_timeBetween)
		{
			float r3 = m_randomGen.getF32(m_minSpeed*0.5f, m_maxSpeed*0.5f);
			FW::Vec2f rndUnitSquare = m_randomGen.getVec2f(0.0f,1.0f);
			FW::Vec2f rndUnitDisk = toUnitDisk(rndUnitSquare);
			FW::Mat3f formBasisMat = formBasis(FW::Vec3f(0.0f, 1.0f, 0.0f));
			FW::Vec3f rndToUnitHalfSphere = FW::Vec3f(rndUnitDisk.x, rndUnitDisk.y, FW::sqrt(1.0f-(rndUnitDisk.x*rndUnitDisk.x)-(rndUnitDisk.y*rndUnitDisk.y)));
			FW::Vec3f v = r3*(formBasisMat*rndToUnitHalfSphere);
			Particle<System>* particle = new (((MemPool*) c.data1)->alloc()) Particle<System>(1.0f, m_carrier->getPos(), v, m_lifetime);
			c.createdActors.push_back(particle);
			tmp -= m_timeBetween;
		}
		m_previousTick = tmp;
		Runge_KuttaIntegrator integrator = Runge_KuttaIntegrator::get();
		integrator.evalIntegrator(dt, m_carrier, c);
		m_carrier->updateStateFromBuffer();
		setState(m_carrier->getState());
		return StateEval();
	}

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
	if(!m_hasVortexEffect)
		renderer.drawTriangleToCamera(m_state.pos, FW::Vec4f(1.0f, 1.0f, 0.0f, 1.0f));
	else
		renderer.drawTriangleToCamera(m_state.pos, FW::Vec4f(.0f, 1.0f, .0f, 1.0f));
}

template<typename System>
void ParticleEmitter<System>::draw(Renderer& renderer)
{
	renderer.drawTriangleToCamera(m_carrier->getPos(), FW::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
}

template<typename System>
void Vortex<System>::draw(Renderer& renderer)
{
	if(!m_hasVortexEffect)
		renderer.drawTriangleToCamera(m_carrier->getPos(), FW::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
	else
		renderer.drawTriangleToCamera(m_carrier->getPos(), FW::Vec4f(1.0f, .0f, 1.0f, 1.0f));
}