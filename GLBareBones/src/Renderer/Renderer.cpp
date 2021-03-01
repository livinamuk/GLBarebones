#include "Renderer.h"
#include "Renderer/TextBlitter.h"

Shader Renderer::s_test_shader;
Shader Renderer::s_solid_color_shader;
Shader Renderer::s_textued_2D_quad_shader;
Shader Renderer::s_screen_space_decal_shader;
Camera* Renderer::p_camera;
Entity Renderer::s_couchEntity;
Model Renderer::s_couchModel;
GBuffer Renderer::s_gBuffer;

void Renderer::Init()
{
    s_test_shader = Shader("Test Shader", "test.vert", "test.frag");
    s_solid_color_shader = Shader("Solid Color Shader", "solidColor.vert", "solidColor.frag");
    s_textued_2D_quad_shader = Shader("Textured 2D Quad Shader", "textured2DquadShader.vert", "textured2DquadShader.frag");
    s_screen_space_decal_shader = Shader("Screen Space Decals", "decals.vert", "decals.frag");

    Texture::LoadTexture("Couch_ALB.png");
    Texture::LoadTexture("CharSheet.png");
    Texture::LoadTexture("Decal.png");

    s_couchModel = Model("Couch.obj");
    s_couchEntity = Entity(&s_couchModel, Texture::GetTexIDByTexName("Couch_ALB.png"));
    s_couchEntity.m_transform.position.z -= 1;
    s_couchEntity.m_transform.scale = glm::vec3(0.07000000029802323f);
    s_couchEntity.UpdateCollisionObject();

    s_gBuffer = GBuffer(CoreGL::s_windowWidth, CoreGL::s_windowHeight);
}

void Renderer::RenderFrame()
{

    glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
    glClearColor(1.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DrawGrid(&s_solid_color_shader);

    DrawCouch(&s_test_shader);

    DrawScreenSpaceDecals();
    
    TextBlitterPass(&s_textued_2D_quad_shader);

    RenderFinalImage();

    // Bullet Debug
    if (Input::s_showBulletDebug) {
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);
        s_solid_color_shader.use();
        s_solid_color_shader.setMat4("projection", p_camera->m_projectionMatrix);
        s_solid_color_shader.setMat4("view", p_camera->m_viewMatrix);
        Physics::DebugDraw(&s_solid_color_shader);
    }
}

void Renderer::DrawGrid(Shader* shader)
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader->ID);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    p_camera->CalculateProjectionMatrix(SCR_WIDTH, SCR_HEIGHT);
    p_camera->CalculateMatrices();

    shader->use();
    shader->setMat4("projection", p_camera->m_projectionMatrix);
    shader->setMat4("view", p_camera->m_viewMatrix);
    shader->setMat4("model", glm::mat4(1));

    int grid_size = 20;
    float grid_scale = 0.25;
    for (int x = -grid_size / 2; x < grid_size / 2 + 1; x++) {
        for (int z = -grid_size / 2; z < grid_size / 2 + 1; z++) {
            Line line;
            line.start_color = glm::vec3(0.5);
            line.end_color = glm::vec3(0.5);
            line.start_pos = glm::vec3(x * grid_scale, 0, -grid_size / 2 * grid_scale);
            line.end_pos = glm::vec3(x * grid_scale, 0, grid_size / 2 * grid_scale);
            DrawLine(shader, line, glm::mat4(1));
            line.start_color = glm::vec3(0.5);
            line.end_color = glm::vec3(0.5);
            line.start_pos = glm::vec3(-grid_size / 2 * grid_scale, 0, z * grid_scale);
            line.end_pos = glm::vec3(grid_size / 2 * grid_scale, 0, z * grid_scale);
            DrawLine(shader, line, glm::mat4(1));
        }
    }
}

void Renderer::DrawCouch(Shader* shader)
{
    glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
    glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

    glEnable(GL_DEPTH_TEST);

    shader->use();
    shader->setMat4("projection", p_camera->m_projectionMatrix);
    shader->setMat4("view", p_camera->m_viewMatrix);

    s_couchEntity.DrawEntity(shader);
}

void Renderer::TextBlitterPass(Shader* shader)
{
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexIDByTexName("CharSheet.png"));
    TextBlitter::BlitText(TextBlitter::s_textToBlit, false); // this congfigures internal blitter variables
    TextBlitter::DrawTextBlit(shader);                       // this draws the thing
    TextBlitter::ResetBlitter();                             // resets internal variables
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

}

void Renderer::DrawLine(Shader* shader, Line line, glm::mat4 modelMatrix = glm::mat4(1))
{
    static unsigned int VAO = 0;
    static unsigned int VBO;

    if (VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
    }

    float vertices[] = {
        line.start_pos.r,  line.start_pos.g,  line.start_pos.b,  line.start_color.r,  line.start_color.g,  line.start_color.b,
        line.end_pos.r,  line.end_pos.g,  line.end_pos.b,  line.end_color.r,  line.end_color.g,  line.end_color.b,
    };

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    shader->setMat4("model", modelMatrix);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 6);
}

void Renderer::HotLoadShaders()
{
    s_test_shader.ReloadShader();
    s_screen_space_decal_shader.ReloadShader();
}

void Renderer::DrawScreenSpaceDecals()
{
    // Draw screenspace decals
    s_screen_space_decal_shader.use();

    glDepthMask(GL_TRUE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glEnable(GL_DEPTH_TEST);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glBindFramebuffer(GL_FRAMEBUFFER, s_gBuffer.ID);
    glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //	glDisable(GL_CULL_FACE);
        /// Now render the decals

    s_screen_space_decal_shader.setMat4("pv", p_camera->m_projectionViewMatrix);
    s_screen_space_decal_shader.setMat4("inverseProjectionMatrix", glm::inverse(p_camera->m_projectionMatrix));
    s_screen_space_decal_shader.setMat4("inverseViewMatrix", glm::inverse(p_camera->m_viewMatrix));
    s_screen_space_decal_shader.setFloat("screenWidth", CoreGL::s_windowWidth);
    s_screen_space_decal_shader.setFloat("screenHeight", CoreGL::s_windowHeight);
    s_screen_space_decal_shader.setVec3("u_CameraFront", p_camera->m_Front);
    s_screen_space_decal_shader.setVec3("u_ViewPos", p_camera->m_viewPos);
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_gBuffer.rboDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture::GetTexIDByTexName("Decal.png"));

    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (Decal decal : Decal::s_decals) {
        s_screen_space_decal_shader.setVec3("targetPlaneSurfaceNormal", decal.normal);
        decal.Draw(&s_screen_space_decal_shader, true);
    }
    glDepthMask(GL_TRUE);




    // Draw decal line
    s_solid_color_shader.use();
    for (Decal& decal : Decal::s_decals)
    {
        Line line;
        line.start_color = glm::vec3(1, 0, 0);
        line.end_color = glm::vec3(1, 1, 1);
        line.start_pos = decal.transform.position;
        line.end_pos = decal.transform.position + (decal.normal * 0.1f);
        DrawLine(&s_solid_color_shader, line, glm::mat4(1));
    }
}

void Renderer::RenderFinalImage()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(s_textued_2D_quad_shader.ID);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, s_gBuffer.gAlbedo);
    glViewport(0, 0, CoreGL::s_windowWidth, CoreGL::s_windowHeight);

    static GLuint VAO = 0;
    if (VAO == 0)
    {
        float quadVertices[] = {
            // positions         texcoords
            -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
                1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        };
        unsigned int VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    s_textued_2D_quad_shader.setMat4("model", glm::mat4(1));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}