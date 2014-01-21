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

		void startUp(FW::GLContext*, FW::CameraControls*, AssetManager*);
        void shutDown();

		void startFrame(const float );
		void endFrame();
		void drawParticle(const FW::Vec3f); 
		void renderTest(const float);
private:
		FW::Mat4f m_projection;
		FW::Mat4f m_worldToCamera;
		FW::Mat4f m_meshScale;
        FW::GLContext* m_context;
		FW::CameraControls* m_camera;
        AssetManager* m_assetManager;
        FW::GLContext::Program* m_shader;

};