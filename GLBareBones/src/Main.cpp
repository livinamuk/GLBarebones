#include "Header.h"
#include "Core/CoreGL.h"
#include "Core/Input.h"
#include "Core/Camera.h"
#include "Core/Entity.h"
#include "Helpers/Util.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "Renderer/TextBlitter.h"
#include "Renderer/Renderer.h"
#include "Physics/physics.h"

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
    
        // Hotload shader
        if (Input::s_keyPressed[HELL_KEY_H])
            Renderer::HotLoadShaders();
        
        // Text Blit Pass
        TextBlitter::BlitLine("Show Bullet Debug: " + Util::BoolToString(Input::s_showBulletDebug));
        TextBlitter::BlitLine("Fullscreen: " + Util::BoolToString(CoreGL::IsFullscreen()));

        // RENDER FRAME
        Renderer::RenderFrame();
        CoreGL::SwapBuffersAndPollEvents();
    }

    CoreGL::Terminate();
    return 0;
}

