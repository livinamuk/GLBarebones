#pragma once
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Model.h"
#include "Core/Camera.h"
#include "Core/CoreGL.h"
#include "Core/Entity.h"
#include "Physics/physics.h"

class Renderer
{
	
public: // functions
	static void Init();
	static void RenderFrame();
	static void DrawGrid(Shader* shader);
	static void DrawCouch(Shader* shader);
	static void TextBlitterPass(Shader* shader);
	static void DrawLine(Shader* shader, Line line, glm::mat4 modelMatrix);
	static void HotLoadShaders();

public: // variables
	static Texture s_charSheetTexture;
	static Shader s_test_shader;
	static Shader s_solid_color_shader;
	static Shader s_textued_2D_quad_shader;
	static Camera* p_camera;

	static Model s_couchModel;
	static Entity s_couchEntity;
};

