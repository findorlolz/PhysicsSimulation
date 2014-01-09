#pragma once

#include "AssetManager.h"
#include "3d/CameraControls.hpp"

class Renderer
{
public:
        Renderer() {}
        ~Renderer() {}

		void startUp(FW::GLContext*, FW::CameraControls*, AssetManager*);
        void shutDown();

        void render(const std::vector<FW::Vec3f>&);

private:
        FW::GLContext* m_context;
		FW::CameraControls* m_camera;
        AssetManager* m_assetManager;
        FW::GLContext::Program* m_shader;
		FW::Mat4f m_projection;
};