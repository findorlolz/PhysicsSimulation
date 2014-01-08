#pragma once

#include "gui/Window.hpp"
#include "gui/CommonControls.hpp"
#include "Render.h"

namespace FW
{

class App : public Window::Listener
{
private:
    enum Action
    {
        Action_None,
		Action_Temp
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
	void reShapeFunc();

private:
	Window m_window;
	CommonControls m_commonCtrl;
	Action m_action;
	Renderer* m_renderer;
	AssetManager* m_assetManager;
	Camera*	m_camera;
	std::vector<FW::Vec3f> m_positions;

};

}