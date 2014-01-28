#pragma once

#include "AssetManager.h"
#include "3d/CameraControls.hpp"

class Renderer
{
public:
        Renderer() {}
        ~Renderer() {}

        static Renderer& get()
        {
                static Renderer* gpSingleton = nullptr;
                if (gpSingleton == nullptr)
                {
                        gpSingleton = new Renderer();
                }
                FW_ASSERT(gpSingleton != nullptr && "Failed to create Renderer");
                return *gpSingleton;
        }

		void startUp(FW::GLContext*, FW::CameraControls*, AssetManager*, bool);
        void shutDown();

		void startFrame(const float );
		void endFrame();
		
		void toggleAxis() { m_drawAxis = !m_drawAxis; }

		void drawParticle(const FW::Vec3f);
		void drawPyramid(const FW::Vec3f pos);

private:
		FW::Mat4f m_projection;
		FW::Mat4f m_worldToCamera;
		FW::Mat4f m_meshScale;
        FW::GLContext* m_context;
		FW::CameraControls* m_camera;
        AssetManager* m_assetManager;
        FW::GLContext::Program* m_shader;
		bool m_drawAxis;

};