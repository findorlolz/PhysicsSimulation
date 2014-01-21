#include "App.hpp"
#include "base/Main.hpp"

using namespace FW;

App::App( void ) : 
	m_commonCtrl( CommonControls::Feature_Default & ~CommonControls::Feature_RepaintOnF5 ),
	m_cameraCtrl(&m_commonCtrl, CameraControls::Feature_All),
	m_action( Action_None ),
	m_scale(0.01f),
	m_stepSize(0.111f),
	m_lastFrameTick(0.0f),
	m_system(nullptr),
	m_stateChange( false ),
	m_visibleCameraControls(false)
{
	m_commonCtrl.showFPS(true);

	m_window.setTitle("Application");
    m_window.addListener(this);
    m_window.addListener(&m_commonCtrl);
	m_window.addListener(&m_cameraCtrl);

	m_commonCtrl.addSeparator();
	m_commonCtrl.addButton((S32*)&m_action, Action_ToggleCameraCtrlVisibility, FW_KEY_F2, "Toggle visibility of camera controls");
	m_commonCtrl.addButton((S32*)&m_action, Action_EnableCamera, FW_KEY_F1, "Enable/Disable camera movements");
	m_commonCtrl.addSeparator();
	m_commonCtrl.addButton((S32*)&m_action, Action_StartParticleSystem, FW_KEY_1, "Create new particles system" );
	m_commonCtrl.addButton((S32*)&m_action, Action_StartBoidSystem, FW_KEY_2, "Create new boid system" );
	m_commonCtrl.beginSliderStack();
	m_commonCtrl.addSlider(&m_scale, 0.0001f, 1.0f, true, FW_KEY_NONE, FW_KEY_NONE, "Scale meshes");
	m_commonCtrl.addSlider(&m_stepSize, 0.0001f, 1.0f, true, FW_KEY_NONE, FW_KEY_NONE, "Step size = %f ");
	m_commonCtrl.endSliderStack();
	m_cameraCtrl.removeGUIControls();

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
		Renderer::get().startUp(m_window.getGL(), &m_cameraCtrl, m_assetManager);
		m_timer = Timer();
}


void App::shutDown()
{
	
	Renderer::get().shutDown();
	delete &EulerIntegrator::get();

	if(m_assetManager != nullptr)
	{
		m_assetManager->ReleaseAssets();
		delete m_assetManager;
	}

	if(m_system != nullptr)
	{
		delete m_system;
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
	
	case Action_EnableCamera:
		m_commonCtrl.message("Enable/Disable camera movements");
		m_cameraCtrl.setEnableMovement(!m_cameraCtrl.getEnableMovement());
		break;

	case Action_StartParticleSystem:
		if(m_system != nullptr)
			delete m_system;
		m_system = new ParticleSystem();
		m_timer.clearTotal();
		m_lastFrameTick = 0.0f;
		break;

	case Action_StartBoidSystem:
		if(m_system != nullptr)
			delete m_system;
		m_system = new BoidSystem(0.2f, 100);
		m_timer.clearTotal();
		m_lastFrameTick = 0.0f;
		break;

	default:
		FW_ASSERT(false);
		break;
	}

	if(m_stateChange)
		updateAppState();

	float tick = m_timer.getElapsed();
	float dt = tick - m_lastFrameTick;
	if(dt >= m_stepSize && m_system != nullptr)
	{
		m_lastFrameTick = tick;
		m_system->evalSystem(dt);
	}

	m_window.setVisible(true);
	if (event.type == Window::EventType_Paint)
	{
		if(m_system != nullptr)
		{
			m_system->draw(m_scale);
		}
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