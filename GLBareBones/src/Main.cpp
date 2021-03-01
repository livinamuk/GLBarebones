#include "Header.h"
#include "Core/CoreGL.h"
#include "Core/Input.h"
#include "Core/Camera.h"
#include "Core/Entity.h"
#include "Helpers/Util.h"
#include "Renderer/Decal.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "Renderer/TextBlitter.h"
#include "Renderer/Renderer.h"
#include "Physics/physics.h"
#include "Physics/RaycastResult.h"

int main()
{
    CoreGL::InitGL(SCR_WIDTH, SCR_HEIGHT);
    Physics::Init();
    Renderer::Init();

    Camera camera;
    CoreGL::SetCamera(&camera);
    Renderer::p_camera = &camera;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
      
    while (CoreGL::IsRunning() && !Input::s_keyDown[HELL_KEY_ESCAPE])
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        camera.Update(deltaTime);

        CoreGL::ProcessInput(); 
        Input::HandleKeypresses();

        // Hotload shader?
        if (Input::s_keyPressed[HELL_KEY_H])
            Renderer::HotLoadShaders();

        // Ray cast every frame
        RaycastResult raycastResult;
        raycastResult.CastRay(camera.m_transform.position, camera.m_Front, 10);

        // Create new decal on mouse click
        if (Input::s_leftMousePressed) {
            Decal::s_decals.push_back(Decal(raycastResult.m_hitPoint, raycastResult.m_surfaceNormal));
        }

        // Text to blit...
        TextBlitter::BlitLine("Show Bullet Debug: " + Util::BoolToString(Input::s_showBulletDebug));
        TextBlitter::BlitLine("Ray Hit: " + Util::PhysicsObjectEnumToString(raycastResult.m_objectType));
        TextBlitter::BlitLine("Decal count: " + std::to_string(Decal::s_decals.size()));

        // RENDER FRAME
        Renderer::RenderFrame();
        CoreGL::SwapBuffersAndPollEvents();
    }

    CoreGL::Terminate();
    return 0;
}

