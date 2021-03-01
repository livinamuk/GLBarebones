#include "Physics.h"
#include "Helpers/Util.h"

btDiscreteDynamicsWorld* Physics::s_dynamicsWorld;
btAlignedObjectArray<btCollisionObject*> Physics::s_collisionObjects;
btAlignedObjectArray<btRigidBody*> Physics::s_rigidBodies;
GLDebugDrawer Physics::s_debugDraw;
btDefaultCollisionConfiguration* Physics::s_collisionConfiguration;
btCollisionDispatcher* Physics::s_dispatcher;
btDbvtBroadphase* Physics::s_broadphase;
btSequentialImpulseConstraintSolver* Physics::s_sol;
btConstraintSolver* Physics::s_solver;
btAlignedObjectArray<btCollisionShape*> Physics::s_collisionShapes;
std::vector<glm::vec3> Physics::s_points;
std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> Physics::s_objectsCollisions;
CollisionPairs Physics::s_pairsLastUpdate;

// Shapes
btBoxShape* Physics::s_windowShape;
btBoxShape* Physics::s_doorShape;
btBoxShape* Physics::s_groundShape;
btCollisionObject* Physics::s_WorldGroundCollisionObject;

void Physics::Init()
{
	s_debugDraw.Init();
	s_broadphase = new btDbvtBroadphase();
	s_collisionConfiguration = new btDefaultCollisionConfiguration();
	s_dispatcher = new btCollisionDispatcher(s_collisionConfiguration);
	s_solver = new btSequentialImpulseConstraintSolver;

	s_dynamicsWorld = new btDiscreteDynamicsWorld(s_dispatcher, s_broadphase, s_solver, s_collisionConfiguration);
	s_dynamicsWorld->setGravity(btVector3(0, -10, 0));
	s_dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	s_dynamicsWorld->setDebugDrawer(&s_debugDraw);
	s_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_FastWireframe + btIDebugDraw::DBG_DrawConstraints + btIDebugDraw::DBG_DrawConstraintLimits);

	//s_groundShape = new btBoxShape(btVector3(0.5, 0.5, 0.5));
	//s_groundShape->setLocalScaling(btVector3(WORLD_GROUND_SIZE, WORLD_GROUND_HEIGHT, WORLD_GROUND_SIZE));
	//CreateWorldGround();
}
/*
void Physics::CreateWorldGround()
{
	// Create the ground
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0, -WORLD_GROUND_HEIGHT, 0));

	float friction = 0.5f;
	int collisionGroup = CollisionGroups::TERRAIN;
	int collisionMask = CollisionGroups::PLAYER | CollisionGroups::PROJECTILES | CollisionGroups::ENEMY;
	btCollisionShape* collisionShape = s_groundShape;
	PhysicsObjectType objectType = PhysicsObjectType::UNDEFINED;

	s_WorldGroundCollisionObject = Physics::CreateCollisionObject(transform, collisionShape, objectType, collisionGroup, collisionMask, friction, DEBUG_COLOR_DOOR, nullptr);
}*/

void Physics::DebugDraw(Shader* shader)
{
	s_debugDraw.lines.clear();
	s_debugDraw.vertices.clear();
	Physics::s_dynamicsWorld->debugDrawWorld();

	for (int i = 0; i < s_debugDraw.lines.size() - 1; i++)
	{
		Vertex vert0, vert1;
		vert0.Position = s_debugDraw.lines[i].start_pos;
		vert0.Normal = s_debugDraw.lines[i].start_color;	// Yes. You are storing the color in the vertex normal spot.
		vert1.Position = s_debugDraw.lines[i].end_pos;
		vert1.Normal = s_debugDraw.lines[i].end_color;
		s_debugDraw.vertices.push_back(vert0);
		s_debugDraw.vertices.push_back(vert1);

	};

	glBindBuffer(GL_ARRAY_BUFFER, s_debugDraw.VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_debugDraw.vertices.size(), &s_debugDraw.vertices[0], GL_STATIC_DRAW);

	glBindVertexArray(s_debugDraw.VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));


	shader->setVec3("color", s_debugDraw.lines[0].start_color);
	shader->setMat4("model", glm::mat4(1));
	glBindVertexArray(s_debugDraw.VAO);
	glDrawArrays(GL_LINES, 0, s_debugDraw.vertices.size());
}

btCollisionObject* Physics::CreateCollisionObject(btTransform transform, btCollisionShape* collisionShape, PhysicsObjectType objectType, int collisionGroup, int collisionMask, float friction, btVector3 debugColor, void* parent)
{
	btCollisionObject* collisionObject = new btCollisionObject();
	collisionObject->setCollisionShape(collisionShape);
	collisionObject->setWorldTransform(transform);
	collisionObject->setCustomDebugColor(debugColor);

	EntityData* entityData = new EntityData();
	entityData->type = objectType;
	entityData->ptr = parent;
	collisionObject->setUserPointer(entityData);

	collisionObject->setCollisionFlags(collisionObject->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	s_dynamicsWorld->addCollisionObject(collisionObject, collisionGroup, collisionMask);

	return collisionObject;
}

btRigidBody* Physics::createRigidBody(float mass, const btTransform& startTransform, btCollisionShape* shape, float friction, int group, int mask)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo cInfo(mass, myMotionState, shape, localInertia);
	cInfo.m_friction = friction;

	btRigidBody* body = new btRigidBody(cInfo);
	//body->setContactProcessingThreshold(m_defaultContactProcessingThreshold);

#else
	btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
	body->setWorldTransform(startTransform);
#endif  //

	body->setUserIndex(-1);
	s_dynamicsWorld->addRigidBody(body, group, mask);

	return body;

}

void Physics::DeleteRigidBody(btRigidBody*& rigidBody)
{
	//if (rigidBody->getMotionState())
	delete rigidBody->getMotionState();

	//if (rigidBody->getCollisionShape())
	delete rigidBody->getCollisionShape();

	s_dynamicsWorld->removeRigidBody(rigidBody);
	delete rigidBody;
	rigidBody = nullptr;
}


void Physics::Update(float deltaTime)
{
	int maxSubSteps = 1;
	btScalar fixedTimeStep = btScalar(1.) / btScalar(60.);

	//maxSubSteps = 10;
	//fixedTimeStep = 1.0 / 240.0;
	s_dynamicsWorld->stepSimulation(deltaTime, maxSubSteps, fixedTimeStep);
}

void Physics::SetRigidBodyWorldTransform(btRigidBody& rigidBody, Transform& transform)
{
	// position and rotation
	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin(Util::glmVec3_to_btVec3(transform.position));

	glm::quat r = glm::quat(transform.rotation);
	btTrans.setRotation(btQuaternion(r.x, r.y, r.z, r.w));

	rigidBody.setWorldTransform(btTrans);

	// scale
	rigidBody.getCollisionShape()->setLocalScaling(Util::glmVec3_to_btVec3(transform.scale));
}

void Physics::SetCollisionObjectWorldTransform(btCollisionObject* collisionObject, Transform& transform)
{
	// position and rotation
	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin(Util::glmVec3_to_btVec3(transform.position));

	glm::quat r = glm::quat(transform.rotation);
	btTrans.setRotation(btQuaternion(r.x, r.y, r.z, r.w));

	collisionObject->setWorldTransform(btTrans);

	// scale
	collisionObject->getCollisionShape()->setLocalScaling(Util::glmVec3_to_btVec3(transform.scale));
}

void Physics::SetCollisionObjectWorldTranslation(btCollisionObject* collisionObject, const glm::vec3& position, glm::vec3 orginOffset)
{
	btTransform btTrans;
	btTrans = collisionObject->getWorldTransform();
	btTrans.setOrigin(Util::glmVec3_to_btVec3(position + orginOffset));
	collisionObject->setWorldTransform(btTrans);
}

void Physics::SetCollisionObjectWorldRotation(btCollisionObject* collisionObject, const glm::vec3& rotation)
{
	btTransform btTrans;
	btTrans = collisionObject->getWorldTransform();
	btTrans.setRotation(Util::glmVec3_to_btQuat(rotation));
	collisionObject->setWorldTransform(btTrans);
}
