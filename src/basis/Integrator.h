#pragma once

#include <vector>
#include "base/Math.hpp"

class ActorContainer;
class StateEval;
class Actor;

class EulerIntegrator
{
public:
	EulerIntegrator() {}
    ~EulerIntegrator() {}

	static EulerIntegrator& get();
	void evalIntegrator(const float, ActorContainer&);
};


class Runge_KuttaIntegrator
{
public:
	Runge_KuttaIntegrator() {}
    ~Runge_KuttaIntegrator() {}

	static Runge_KuttaIntegrator& get();
	void evalIntegrator(const float, ActorContainer&);
	void evalIntegrator(const float, Actor*, ActorContainer&);
};

