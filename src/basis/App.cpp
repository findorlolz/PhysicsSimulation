#include "App.hpp"
#include "base/Main.hpp"

using namespace FW;

App::App( void ) : 
	m_commonCtrl( CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5 ),
	m_cameraCtrl(&m_commonCtrl, CameraControls::Feature_All),
	m_action( Action_None ),
	m_stateChange( false ),
	m_visibleCameraControls(false)
{
	m_commonCtrl.showFPS(true);

	m_window.setTitle("Application");
    m_window.addListener(this);
    m_window.addListener(&m_commonCtrl);
	m_window.addListener(&m_cameraCtrl);

	m_commonCtrl.addButton((S32*)&m_action, Action_ToggleCameraCtrlVisibility, FW_KEY_F1, "Toggle visibility of camera controls");
	m_commonCtrl.addSeparator();

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

        m_renderer = new Renderer();
		m_renderer->startUp(m_window.getGL(), &m_cameraCtrl, m_assetManager);

		m_positions.push_back(FW::Vec3f());
		m_positions.push_back(FW::Vec3f(1,1,0));
}


void App::shutDown()
{
	if(m_renderer != nullptr)
	{
		m_renderer->shutDown();
		delete m_renderer;
	}
	if(m_assetManager != nullptr)
	{
		m_assetManager->ReleaseAssets();
		delete m_assetManager;
	}
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

	case Action_ToggleCameraCtrlVisibility:
		m_commonCtrl.message("Toggle visibility of camera controls");
		m_visibleCameraControls = !m_visibleCameraControls;
		m_stateChange = true;
		break;

	default:
		FW_ASSERT(false);
		break;
	}

	if(m_stateChange)
		updateAppState();
	

	m_window.setVisible(true);
	if (event.type == Window::EventType_Paint)
	{
		m_renderer->render(m_positions);

	}
	m_window.repaint();

	return false;
}

void App::updateAppState()
{
	if(m_visibleCameraControls)
		m_cameraCtrl.addGUIControls();
	else
		m_cameraCtrl.removeGUIControls();
	m_stateChange = false;
}

void FW::init(void) 
{
    new App;
}