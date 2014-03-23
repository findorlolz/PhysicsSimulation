#pragma once

#include "gui/Window.hpp"
#include "gui/CommonControls.hpp"
#include "System.h"
#include "Renderer.h"
#include "Integrator.h"

namespace FW
{

class App : public Window::Listener
{
private:
    enum Action
    {
        Action_None,
		Action_EnableCamera,
		Action_ToggleCameraCtrlVisibility,
		Action_StartParticleSystem,
		Action_StartBoidSystem,
		Action_StartFlowSystem,
		Action_ToggleAxis,
		Action_ExportDynamicMesh
	};

public:
	App( void );
	virtual ~App( void ) {}
	virtual bool handleEvent( const Window::Event& event );

private:
	App( const App& ); // forbidden
    App& operator=( const App& ); // forbidden

	void startUp();
	void shutDown();
	void updateAppState();

private:
	Window m_window;
	CommonControls m_commonCtrl;
	CameraControls	m_cameraCtrl;
	bool m_visibleCameraControls;
	bool m_stateChange;
	float m_scale;
	float m_stepSize;
	float m_lastFrameTick;

	Action m_action;
	AssetManager* m_assetManager;

	System* m_system;
	FW::Timer m_timer;
};

}