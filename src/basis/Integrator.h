#pragma once

#include <vector>
#include "base/Math.hpp"

class Actor;

class EulerIntegrator
{
public:
	EulerIntegrator() {}
    ~EulerIntegrator() {}

	static EulerIntegrator& get();
	void evalIntegrator(const float dt, Actor*, std::vector<Actor*>&);
};


class Runge_KuttaIntegrator
{
public:
	Runge_KuttaIntegrator() {}
    ~Runge_KuttaIntegrator() {}

	static Runge_KuttaIntegrator& get();
	void evalIntegrator(const float dt, Actor*, std::vector<Actor*>&);
};

