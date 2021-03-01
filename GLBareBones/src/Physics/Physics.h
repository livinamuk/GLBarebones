#pragma once
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/btBulletCollisionCommon.h"
#include "bullet/src/LinearMath/btVector3.h"
#include "bullet/src/LinearMath/btAlignedObjectArray.h"
#include "bullet/src/BulletCollision/CollisionDispatch/btGhostObject.h"
#include "bullet/src/BulletDynamics/Character/btKinematicCharacterController.h"


#include "Renderer/GLDebugDraw.h"
//#include "Renderer/Cube.h"
//#include "House/House.h"
//#include "Core/AnimatedEntity.h"

//#include "Ragdoll.h"
#include <set>
#include <iterator>
#include <algorithm>
#include <map>

typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionPair;
typedef std::set<CollisionPair> CollisionPairs;

class Physics
{
public: // fields
	static CollisionPairs s_pairsLastUpdate;
		
	static GLDebugDrawer s_debugDraw;
	static btDefaultCollisionConfiguration* s_collisionConfiguration;
	static btCollisionDispatcher* s_dispatcher;
	static btDbvtBroadphase* s_broadphase;
	static btSequentialImpulseConstraintSolver* s_sol;
	static btConstraintSolver* s_solver;
	static btAlignedObjectArray<btCollisionShape*> s_collisionShapes;
	static btDiscreteDynamicsWorld* s_dynamicsWorld;
	static btAlignedObjectArray<btRigidBody*> s_rigidBodies;
	static btAlignedObjectArray<btCollisionObject*> s_collisionObjects;
	static std::vector<glm::vec3> s_points;
	static std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> s_objectsCollisions;


public: // methods 
	static void Init();
	static void Update(float deltaTime);
	static void DebugDraw(Shader* shader);
	static btRigidBody* createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction, int group, int mask);
	static btCollisionObject* CreateCollisionObject(btTransform transform, btCollisionShape* collisionShape, PhysicsObjectType objectType, int collisionGroup, int collisionMask, float friction, btVector3 debugColor,	void* partent);
	static void DeleteRigidBody(btRigidBody*& rigidBody);
	static glm::mat4 GetModelMatrixFromRigidBody(btRigidBody* rigidBody);
	static void SetRigidBodyWorldTransform(btRigidBody& rigidBody, Transform& transform);
	static void SetCollisionObjectWorldTransform(btCollisionObject* collisionObject, Transform& transform);
	static void SetCollisionObjectWorldTranslation(btCollisionObject* collisionObject, const glm::vec3& position, glm::vec3 orginOffset = glm::vec3(0));
	static void SetCollisionObjectWorldRotation(btCollisionObject* collisionObject, const glm::vec3& rotation);

public: // Methods
	//static void CreateWorldGround();
	static btBoxShape* s_windowShape;
	static btBoxShape* s_doorShape;
	static btBoxShape* s_groundShape;
	static btCollisionObject* s_WorldGroundCollisionObject;

};

class OpenGLMotionState : public btDefaultMotionState
{
public:
	OpenGLMotionState(const btTransform& transform) : btDefaultMotionState(transform) {}

	void GetWorldTransform(btScalar* transform)
	{
		btTransform trans;
		getWorldTransform(trans);
		trans.getOpenGLMatrix(transform);
	}
};