#include "App.hpp"
#include "base/Main.hpp"

using namespace FW;

App::App( void ) : 
	m_commonCtrl( CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5 ),
	m_action( Action_None )
{
	m_commonCtrl.showFPS(true);

	m_window.setTitle("Application");
    m_window.addListener(this);
    m_window.addListener(&m_commonCtrl);

	FW::GLContext::Config conf = m_window.getGLConfig();
	conf.numSamples = 4;
	m_window.setGLConfig(conf);
	m_window.getGL()->swapBuffers();

	startUp();
}

void App::startUp()
{
        m_assetManager = new AssetManager();
        m_assetManager->LoadAssets();

		m_camera = new Camera();
		m_camera->StartUp();
		reShapeFunc();

        m_renderer = new Renderer();
		m_renderer->startUp(m_window.getGL(), m_camera, m_assetManager);

		m_positions.push_back(FW::Vec3f());
}

void App::reShapeFunc()
{
	FW::Vec2i size = m_window.getSize();
	m_camera->SetDimensions(size.x, size.y);
	m_camera->SetViewport(0,0,size.x,size.y);
	m_camera->ApplyViewport();
	m_camera->SetPerspective(90);
}

void App::shutDown()
{
	if(m_renderer != nullptr)
		delete m_renderer;
	if(m_assetManager != nullptr)
		delete m_assetManager;
	if(m_camera != nullptr)
		delete m_camera;
}

bool App::handleEvent( const Window::Event& event )
{
	if (event.type == Window::EventType_Close)
	{
		m_window.showModalMessage("Exiting...");
		shutDown();
		delete this;
		return true;
	}

	Action action = m_action;
	m_action = Action_None;

	switch (action)
	{
	case Action_None:
		break;

	case Action_Temp:
		m_commonCtrl.message("Temp action");
		break;

	default:
		FW_ASSERT(false);
		break;
	}

	m_window.setVisible(true);
	if (event.type == Window::EventType_Paint)
	{
		reShapeFunc();
		m_renderer->render(m_positions);

	}
	m_window.repaint();

	return false;
}

void FW::init(void) 
{
    new App;
}