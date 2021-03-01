//#include "hellpch.h"
#include "Util.h"
#include <Windows.h>
#include <random>

void Util::PrintVec3(glm::vec3 v)
{
	std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

void Util::PrintMat4(glm::mat4 m)
{
	std::cout << "(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " << m[0][3] << ")\n";
	std::cout << "(" << m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " << m[1][3] << ")\n";
	std::cout << "(" << m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ", " << m[2][3] << ")\n";
	std::cout << "(" << m[3][0] << ", " << m[3][1] << ", " << m[3][2] << ", " << m[3][3] << ")\n";
}

std::string Util::FileNameFromPath(std::string filepath)
{
	// Remove directory if present.
	// Do this before extension removal incase directory has a period character.
	const size_t last_slash_idx = filepath.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		filepath.erase(0, last_slash_idx + 1);
	}

	// Remove extension if present.
	const size_t period_idx = filepath.rfind('.');
	if (std::string::npos != period_idx)
	{
		filepath.erase(period_idx);
	}
	return filepath;
}
FileType Util::FileTypeFromPath(std::string filepath)
{
	int index = filepath.find_last_of('.') + 1;
	std::string extension = filepath.substr(index);
	//std::cout << "'" << extension << "'\n";

	if (extension == "PNG" || extension == "png")
		return FileType::PNG;
	if (extension == "JPG" || extension == "jpg")
		return FileType::JPG;
	if (extension == "FBX" || extension == "fbx")
		return FileType::FBX;
	if (extension == "OBJ" || extension == "obj")
		return FileType::OBJ;
	else
		return FileType::UNKNOWN;
}

void Util::SetNormalsAndTangentsFromVertices(Vertex* vert0, Vertex* vert1, Vertex* vert2)
{
	// above can be used to replace the shit below here. its the same.

	// Shortcuts for UVs
	glm::vec3& v0 = vert0->Position;
	glm::vec3& v1 = vert1->Position;
	glm::vec3& v2 = vert2->Position;
	glm::vec2& uv0 = vert0->TexCoords;
	glm::vec2& uv1 = vert1->TexCoords;
	glm::vec2& uv2 = vert2->TexCoords;

	// Edges of the triangle : postion delta. UV delta
	glm::vec3 deltaPos1 = v1 - v0;
	glm::vec3 deltaPos2 = v2 - v0;
	glm::vec2 deltaUV1 = uv1 - uv0;
	glm::vec2 deltaUV2 = uv2 - uv0;

	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
	glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

	glm::vec3 normal = Util::NormalFromTriangle(vert0->Position, vert1->Position, vert2->Position);

	vert0->Normal = normal;
	vert1->Normal = normal;
	vert2->Normal = normal;

	vert0->Tangent = tangent;
	vert1->Tangent = tangent;
	vert2->Tangent = tangent;

	vert0->Bitangent = bitangent;
	vert1->Bitangent = bitangent;
	vert2->Bitangent = bitangent;
}

glm::vec3 Util::NormalFromTriangle(glm::vec3 pos0, glm::vec3 pos1, glm::vec3 pos2)
{
	return glm::normalize(glm::cross(pos1 - pos0, pos2 - pos0));
}

btVector3 Util::glmVec3_to_btVec3(glm::vec3 vector)
{
	return btVector3(vector.x, vector.y, vector.z);
}

btQuaternion Util::glmVec3_to_btQuat(glm::vec3 vector)
{
	glm::quat r = glm::quat(vector);
	return btQuaternion(r.x, r.y, r.z, r.w);
}

glm::vec3 Util::btVec3_to_glmVec3(btVector3 vector)
{
	return glm::vec3(vector.x(), vector.y(), vector.z());
}

std::string Util::BoolToString(bool boolean)
{
	if (boolean)
		return "TRUE";
	else
		return "FALSE";
}

/*char* Util::PhysicsObjectEnumToString(PhysicsObjectType objectType)
{
	if (objectType == PhysicsObjectType::UNDEFINED)
		return "UNDEFINED";
	if (objectType == PhysicsObjectType::WALL)
		return "WALL";
	if (objectType == PhysicsObjectType::FLOOR)
		return "FLOOR";
	if (objectType == PhysicsObjectType::DOOR)
		return "DOOR";
	if (objectType == PhysicsObjectType::STAIRS)
		return "STAIRS";
	if (objectType == PhysicsObjectType::PLAYER)
		return "PLAYER";
	if (objectType == PhysicsObjectType::RAGDOLL)
		return "RAGDOLL";
	if (objectType == PhysicsObjectType::CEILING)
		return "CEILING";
	if (objectType == PhysicsObjectType::SHELL_PROJECTILE)
		return "SHELL_PROJECTILE";
	if (objectType == PhysicsObjectType::MISC_MESH)
		return "MISC_MESH";
	if (objectType == PhysicsObjectType::EDITOR_WINDOW)
		return "WINDOW";
	if (objectType == PhysicsObjectType::GLASS)
		return "GLASS";
}*/