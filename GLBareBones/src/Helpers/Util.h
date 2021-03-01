#pragma once
#include "Header.h"
#include "Renderer/Transform.h"
#include "Renderer/Shader.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletDynamicsCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"

//#include "assimp/matrix4x4.h"
//#include "assimp/matrix3x3.h"

class Util
{
public:
	static void PrintMat4(glm::mat4 m);
	static void PrintVec3(glm::vec3 v);
	static std::string FileNameFromPath(std::string filepath);
	static FileType FileTypeFromPath(std::string filepath);
	static void SetNormalsAndTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2);
	static glm::vec3 NormalFromTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2);

	static btVector3 glmVec3_to_btVec3(glm::vec3 vector);
	static btQuaternion glmVec3_to_btQuat(glm::vec3 vector);
	static glm::vec3 btVec3_to_glmVec3(btVector3);
	static std::string PhysicsObjectEnumToString(PhysicsObjectType objectType);

	static std::string BoolToString(bool boolean);
};
