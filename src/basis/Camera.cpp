#include "camera.h"
#include <cstdio>
#include <iostream>

using namespace std;
using namespace FW;

Camera::Camera()
{
}

Camera::~Camera()
{
}
void Camera::StartUp()
{
	currentRot_.setIdentity();
	startRot_.setIdentity();
}

void Camera::SetDimensions(int w, int h)
{
    dimensions_[0] = w;
    dimensions_[1] = h;
}

void Camera::SetPerspective(float fovy)
{
    perspective_[0] = fovy;
}

void Camera::SetViewport(int x, int y, int w, int h)
{
    viewport_[0] = x;
    viewport_[1] = y;
    viewport_[2] = w;
    viewport_[3] = h;
    perspective_[1] = float( w ) / h;
}

void Camera::SetRotation(const Mat4f& rotation)
{
    startRot_ = currentRot_ = rotation;
}

void Camera::SetDistance(const float distance)
{
    startDistance_ = currentDistance_ = distance;
}

void Camera::MouseClick(MouseButton button, int x, int y)
{
    //Save starting values for cursor when mousebuttons are used
	startClick_[0] = x;
    startClick_[1] = y;

    buttonState_ = button;
    switch (button)
    {
    case LEFT:
        currentRot_ = startRot_;
        break;
    case RIGHT:
        currentDistance_ = startDistance_;
        break;        
    default:
        break;
    }
}

void Camera::MouseDrag(int x, int y)
{
    switch (buttonState_)
    {
    case LEFT:
        ArcBallRotation(x,y);
        break;
    case RIGHT:
        DistanceZoom(x,y);
        break;
    default:
        break;
    }
}


void Camera::MouseRelease()
{
    startRot_ = currentRot_;
    startDistance_ = currentDistance_;
    
    buttonState_ = NONE;
}


void Camera::ArcBallRotation(int x, int y)
{
    float startX, startY, startZ, endX, endY, endZ;
    float scale;
    float startLenght, endLenght;
    float dotprod;
    
	scale = 1.0f/((float) min(dimensions_[0], dimensions_[1]));
	startX = (startClick_[0] - ( dimensions_[0] / 2.f )) *  scale;
	startY = (startClick_[1] - ( dimensions_[1] / 2.f )) * -1.0f * scale;
    endX = (x - ( dimensions_[0] / 2.f )) * scale;
    endY = (y - ( dimensions_[1] / 2.f ))* -1.0f * scale;
    
    //Calculate lenght of the points for origo
	startLenght = (float)hypot(startX, startY);
	endLenght = (float)hypot(endX, endY);
    
    //Set point into unitsquare 
	if (startLenght > 1.f) 
	{
		startX /= startLenght;
		startY /= startLenght;
        startLenght = 1.0;
    }
    if (endLenght > 1.f) {
        endX /= endLenght;
        endY /= endLenght;
        endLenght = 1.f;
    }
    
    startZ = FW::sqrt(1.0f - startLenght * startLenght);
    endZ = FW::sqrt(1.0f - endLenght * endLenght);
    
    dotprod = startX * endX + startY * endY + startZ * endZ;

    if( dotprod != 1 )
    {
        Vec3f axis( startX * endZ - endY * startZ, startZ * endX - endZ * startX, startX * endY - endX * startY);
        axis.normalize();

		float angle = 2.0f * FW::acos( dotprod );

        currentRot_ = rotation4f( axis, angle );
        currentRot_ = currentRot_ * startRot_; 
    }
    else
    {
        currentRot_ = startRot_;
    }


}

void Camera::DistanceZoom(int x, int y)
{
    int startY = startClick_[1] - viewport_[1];
    int currentY = y - viewport_[1];

    float delta = float(currentY-startY)/viewport_[3];

    currentDistance_ = startDistance_ * FW::exp(delta);
	UNREFERENCED_PARAMETER(x);
}

Mat4f Camera::GetPerspective()
{
	static const float fnear = 0.1f, ffar = 4.0f;
	float fo = 1.0f/FW::tan(perspective_[0] * 0.5f);
	Mat4f P;
	P.setCol(0, Vec4f(fo/perspective_[1], 0, 0, 0));
	P.setCol(1, Vec4f(0, fo, 0, 0));
	P.setCol(2, Vec4f(0, 0, (ffar+fnear)/(ffar-fnear), 1));
	P.setCol(3, Vec4f(0, 0, -2*ffar*fnear/(ffar-fnear), 0));
	return P;
}