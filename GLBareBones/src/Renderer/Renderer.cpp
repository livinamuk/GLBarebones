#include "Renderer.h"
#include "Renderer/TextBlitter.h"

Texture Renderer::s_charSheetTexture;
Shader Renderer::s_test_shader;
Shader Renderer::s_solid_color_shader;
Shader Renderer::s_textued_2D_quad_shader;
Camera* Renderer::p_camera;
Entity Renderer::s_couchEntity;
Model Renderer::s_couchModel;

void Renderer::Init()
{
    s_charSheetTexture = Texture("CharSheet.png");
    s_test_shader = Shader("Test Shader", "test.vert", "test.frag");
    s_solid_color_shader = Shader("Solid Color Shader", "solidColor.vert", "solidColor.frag");
    s_textued_2D_quad_shader = Shader("Textured 2D Quad Shader", "textured2DquadShader.vert", "textured2DquadShader.frag");

    Texture couchTexture = Texture("Couch_ALB.png");
    s_couchModel = Model("Couch.obj");
    s_couchEntity = Entity(&s_couchModel, couchTexture.ID);
    s_couchEntity.m_transform.position.z -= 1;
    s_couchEntity.m_transform.scale = glm::vec3(0.07000000029802323f);
    s_couchEntity.UpdateCollisionObject();
}

void Renderer::RenderFrame()
{

    DrawGrid(&s_solid_color_shader);
    DrawCouch(&s_test_shader);
    TextBlitterPass(&s_textued_2D_quad_shader);

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
    glBindTexture(GL_TEXTURE_2D, s_charSheetTexture.ID);
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
}