#pragma once

#include <base/Math.hpp>
#include <gl\GLU.h>
#include "HelpFunctions.h"

using namespace std;
using namespace FW;

class Camera
{
public:
    Camera::Camera();
	Camera::~Camera();
    
	enum MouseButton {NONE,LEFT, RIGHT};
	void StartUp();

    void SetDimensions(int w, int h);
    void SetViewport(int x, int y, int w, int h);
    void SetPerspective(float fovy);

    // Call from whatever UI toolkit
    void MouseClick(MouseButton button, int x, int y);
    void MouseDrag(int x, int y);
    void MouseRelease();

    // Set for relevant vars
    void SetCenter(const FW::Vec3f& center);
    void SetRotation(const FW::Mat4f& rotation);
    void SetDistance(const float distance);

    Mat4f GetRotation() { return currentRot_; }
    float GetDistance() { return currentDistance_; }
	Mat4f GetPerspective();

	//GL interface calls
	void ApplyViewport() {glViewport(viewport_[0],viewport_[1],viewport_[2],viewport_[3]);}

    
private:
 
    int     dimensions_[2];
    int     startClick_[2];
    MouseButton  buttonState_;

    FW::Mat4f startRot_;
    FW::Mat4f currentRot_;
    float   perspective_[2];
    int     viewport_[4];

    float   startDistance_;
    float   currentDistance_;

    void ArcBallRotation(int x, int y);
    void DistanceZoom(int x, int y);
};
