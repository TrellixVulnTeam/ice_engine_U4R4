#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#include "logger/ILogger.hpp"

#include "physics/bullet/PhysicsEngine.hpp"
#include "physics/bullet/utilities/Utilities.hpp"
#include "physics/CollisionShapeHandle.hpp"

namespace hercules
{
namespace physics
{
namespace bullet
{

PhysicsEngine::PhysicsEngine(hercules::utilities::Properties* properties, fs::IFileSystem* fileSystem, logger::ILogger* logger)
{
	properties_ = properties;
	fileSystem_ = fileSystem;
	logger_ = logger;
}

PhysicsEngine::PhysicsEngine(const PhysicsEngine& other)
{
}

PhysicsEngine::~PhysicsEngine()
{
}

void PhysicsEngine::tick(const PhysicsSceneHandle& physicsSceneHandle, const float32 delta)
{
	auto& physicsScene = physicsScenes_[physicsSceneHandle];
	
	physicsScene.dynamicsWorld->stepSimulation(delta, 10);
	
	physicsScene.dynamicsWorld->debugDrawWorld();
}

PhysicsSceneHandle PhysicsEngine::createPhysicsScene()
{
	auto physicsSceneHandle = physicsScenes_.create();
	auto& physicsScene = physicsScenes_[physicsSceneHandle];
	
	physicsScene.broadphase = std::make_unique<btDbvtBroadphase>();
	physicsScene.collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
	physicsScene.dispatcher = std::make_unique<btCollisionDispatcher>(physicsScene.collisionConfiguration.get());
	physicsScene.solver = std::make_unique<btSequentialImpulseConstraintSolver>();
	physicsScene.dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(physicsScene.dispatcher.get(), physicsScene.broadphase.get(), physicsScene.solver.get(), physicsScene.collisionConfiguration.get());
	
	physicsScene.dynamicsWorld->setGravity(btVector3(0, -10, 0));
	physicsScene.dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	
	physicsScene.dynamicsWorld->setDebugDrawer(debugRenderer_.get());
	
	return physicsSceneHandle;
}

void PhysicsEngine::destroyPhysicsScene(const PhysicsSceneHandle& physicsSceneHandle)
{
	physicsScenes_.destroy(physicsSceneHandle);
}

void PhysicsEngine::setGravity(const PhysicsSceneHandle& physicsSceneHandle, const glm::vec3& gravity)
{
	physicsScenes_[physicsSceneHandle].dynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
}

void PhysicsEngine::setPhysicsDebugRenderer(IPhysicsDebugRenderer* physicsDebugRenderer)
{
	debugRenderer_ = std::make_unique<DebugRenderer>(physicsDebugRenderer);
	
	for (auto& scene : physicsScenes_)
	{
		scene.dynamicsWorld->setDebugDrawer(debugRenderer_.get());
	}
}

CollisionShapeHandle PhysicsEngine::createStaticPlaneShape(const glm::vec3& planeNormal, const float32 planeConstant)
{
	auto shape = std::make_unique<btStaticPlaneShape>(btVector3(planeNormal.x, planeNormal.y, planeNormal.z), btScalar(planeConstant));
	
	shapes_.push_back(std::move(shape));
	auto index = shapes_.size() - 1;
	
	return CollisionShapeHandle(index);
}

CollisionShapeHandle PhysicsEngine::createStaticBoxShape(const glm::vec3& dimensions)
{
	auto shape = std::make_unique<btBoxShape>(btVector3(dimensions.x, dimensions.y, dimensions.z));
	
	shapes_.push_back(std::move(shape));
	auto index = shapes_.size() - 1;
	
	return CollisionShapeHandle(index, 1);
}

void PhysicsEngine::destroyStaticShape(const CollisionShapeHandle& collisionShapeHandle)
{
	throw std::logic_error("Method not implemented");
}

void PhysicsEngine::destroyAllStaticShapes()
{
	shapes_.clear();
}

RigidBodyObjectHandle PhysicsEngine::createDynamicRigidBodyObject(
	const PhysicsSceneHandle& physicsSceneHandle,
	const CollisionShapeHandle& collisionShapeHandle,
	std::unique_ptr<IMotionChangeListener> motionStateListener,
	const UserData& userData
)
{
	return createDynamicRigidBodyObject(physicsSceneHandle, collisionShapeHandle, glm::vec3(), glm::quat(), 1.0f, 1.0f, 1.0f, std::move(motionStateListener), userData);
}

RigidBodyObjectHandle PhysicsEngine::createDynamicRigidBodyObject(
	const PhysicsSceneHandle& physicsSceneHandle,
	const CollisionShapeHandle& collisionShapeHandle,
	const float32 mass,
	const float32 friction,
	const float32 restitution,
	std::unique_ptr<IMotionChangeListener> motionStateListener,
	const UserData& userData
)
{
	return createDynamicRigidBodyObject(physicsSceneHandle, collisionShapeHandle, glm::vec3(), glm::quat(), mass, friction, restitution, std::move(motionStateListener), userData);
}

RigidBodyObjectHandle PhysicsEngine::createDynamicRigidBodyObject(
	const PhysicsSceneHandle& physicsSceneHandle,
	const CollisionShapeHandle& collisionShapeHandle,
	const glm::vec3& position,
	const glm::quat& orientation,
	const float32 mass,
	const float32 friction,
	const float32 restitution,
	std::unique_ptr<IMotionChangeListener> motionStateListener,
	const UserData& userData
)
{
	auto& shape = shapes_[collisionShapeHandle.index()];
	
	btTransform transform;
	transform.setRotation( btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w) );
	transform.setOrigin( btVector3(position.x, position.y, position.z) );
	
	btScalar bulletMass(mass);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (bulletMass != btScalar(0.0f))
	{
		shape->calculateLocalInertia(bulletMass, localInertia);
	}
	
	auto rigidBodyData = std::make_unique<BulletRigidBodyData>();
	
	rigidBodyData->motionState = std::make_unique<BulletMotionState>(transform, std::move(motionStateListener));
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(bulletMass, rigidBodyData->motionState.get(), shape.get(), localInertia);
	constructionInfo.m_friction = friction;
	constructionInfo.m_restitution = restitution;
	rigidBodyData->rigidBody = std::make_unique<btRigidBody>(constructionInfo);
	
	rigidBodyData->userData = std::make_unique<BulletUserData>();
	
	rigidBodyData->userData->rigidBodyObjectHandle = RigidBodyObjectHandle(rigidBodyData.get());
	rigidBodyData->userData->type = BulletCollisionObjectType::RIGID_BODY;
	rigidBodyData->userData->userData = userData;
	
	auto rigidBodyDataPointer = rigidBodyData.get();
	auto rigidBodyPointer = rigidBodyData->rigidBody.get();
	
	rigidBodyData_.push_back(std::move(rigidBodyData));
	
	rigidBodyPointer->setUserPointer(rigidBodyDataPointer);
	
	physicsScenes_[physicsSceneHandle].dynamicsWorld->addRigidBody(rigidBodyPointer);
	
	return RigidBodyObjectHandle(rigidBodyDataPointer);
}

RigidBodyObjectHandle PhysicsEngine::createStaticRigidBodyObject(const PhysicsSceneHandle& physicsSceneHandle, const CollisionShapeHandle& collisionShapeHandle, const UserData& userData)
{
	return createStaticRigidBodyObject(physicsSceneHandle, collisionShapeHandle, glm::vec3(), glm::quat(), 1.0f, 1.0f, userData);
}

RigidBodyObjectHandle PhysicsEngine::createStaticRigidBodyObject(
	const PhysicsSceneHandle& physicsSceneHandle,
	const CollisionShapeHandle& collisionShapeHandle,
	const float32 friction,
	const float32 restitution,
	const UserData& userData
)
{
	return createStaticRigidBodyObject(physicsSceneHandle, collisionShapeHandle, glm::vec3(), glm::quat(), friction, restitution, userData);
}

RigidBodyObjectHandle PhysicsEngine::createStaticRigidBodyObject(
	const PhysicsSceneHandle& physicsSceneHandle,
	const CollisionShapeHandle& collisionShapeHandle,
	const glm::vec3& position,
	const glm::quat& orientation,
	const float32 friction,
	const float32 restitution,
	const UserData& userData
)
{
	auto& shape = shapes_[collisionShapeHandle.index()];
	
	btTransform transform;
	transform.setRotation( btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w) );
	transform.setOrigin( btVector3(position.x, position.y, position.z) );
	
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	
	auto rigidBodyData = std::make_unique<BulletRigidBodyData>();
	
	rigidBodyData->motionState = std::make_unique<BulletMotionState>(transform, nullptr);
	btRigidBody::btRigidBodyConstructionInfo constructionInfo(btScalar(0.0f), rigidBodyData->motionState.get(), shape.get(), localInertia);
	constructionInfo.m_friction = friction;
	constructionInfo.m_restitution = restitution;
	rigidBodyData->rigidBody = std::make_unique<btRigidBody>(constructionInfo);
	
	rigidBodyData->userData = std::make_unique<BulletUserData>();
	
	rigidBodyData->userData->rigidBodyObjectHandle = RigidBodyObjectHandle(rigidBodyData.get());
	rigidBodyData->userData->type = BulletCollisionObjectType::RIGID_BODY;
	rigidBodyData->userData->userData = userData;
	
	auto rigidBodyDataPointer = rigidBodyData.get();
	auto rigidBodyPointer = rigidBodyData->rigidBody.get();
	
	rigidBodyData_.push_back(std::move(rigidBodyData));
	
	rigidBodyPointer->setUserPointer(rigidBodyDataPointer);
	
	physicsScenes_[physicsSceneHandle].dynamicsWorld->addRigidBody(rigidBodyPointer);
	
	return RigidBodyObjectHandle(rigidBodyDataPointer);
}

GhostObjectHandle PhysicsEngine::createGhostObject(const PhysicsSceneHandle& physicsSceneHandle, const CollisionShapeHandle& collisionShapeHandle, const UserData& userData)
{
	return createGhostObject(physicsSceneHandle, collisionShapeHandle, glm::vec3(), glm::quat(), userData);
}

GhostObjectHandle PhysicsEngine::createGhostObject(
	const PhysicsSceneHandle& physicsSceneHandle,
	const CollisionShapeHandle& collisionShapeHandle,
	const glm::vec3& position,
	const glm::quat& orientation,
	const UserData& userData
)
{
	auto& shape = shapes_[collisionShapeHandle.index()];
	
	btTransform transform;
	transform.setRotation( btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w) );
	transform.setOrigin( btVector3(position.x, position.y, position.z) );
	
	//btVector3 localInertia(0.0f, 0.0f, 0.0f);
	
	auto ghostObjectData = std::make_unique<BulletGhostObjectData>();
	
	//ghostObjectData->motionState = std::make_unique<BulletMotionState>(transform, nullptr);
	//btRigidBody::btRigidBodyConstructionInfo constructionInfo(btScalar(0.0f), ghostObjectData->motionState.get(), shape.get(), localInertia);
	//constructionInfo.m_friction = friction;
	//constructionInfo.m_restitution = restitution;
	ghostObjectData->ghostObject = std::make_unique<btGhostObject>();
	ghostObjectData->ghostObject->setCollisionShape(shape.get());
	ghostObjectData->ghostObject->setWorldTransform(transform);
	ghostObjectData->ghostObject->setCollisionFlags(ghostObjectData->ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	
	ghostObjectData->userData = std::make_unique<BulletUserData>();
	
	ghostObjectData->userData->ghostObjectHandle = GhostObjectHandle(ghostObjectData.get());
	ghostObjectData->userData->type = BulletCollisionObjectType::GHOST;
	ghostObjectData->userData->userData = userData;
	ghostObjectData->ghostObject->setUserPointer(ghostObjectData->userData.get());
	
	auto ghostDataPointer = ghostObjectData.get();
	auto ghostPointer = ghostObjectData->ghostObject.get();
	
	ghostObjectData_.push_back(std::move(ghostObjectData));
	
	ghostPointer->setUserPointer(ghostDataPointer);
	
	physicsScenes_[physicsSceneHandle].dynamicsWorld->addCollisionObject(ghostPointer);
	//physicsScenes_[physicsSceneHandle].dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
	
	return GhostObjectHandle(ghostDataPointer);
}

void PhysicsEngine::destroyRigidBody(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle)
{
	//rigidBodyData_.destroy(rigidBodyObjectHandle);
}

void PhysicsEngine::destroyAllRigidBodies()
{
	//rigidBodyData_.clear();
}

void PhysicsEngine::setUserData(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const UserData& userData)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	rigidBodyData->userData = std::make_unique<BulletUserData>();
	
	rigidBodyData->userData->rigidBodyObjectHandle = rigidBodyObjectHandle;
	rigidBodyData->userData->type = BulletCollisionObjectType::RIGID_BODY;
	rigidBodyData->userData->userData = userData;
	rigidBodyData->rigidBody->setUserPointer(rigidBodyData->userData.get());
}

void PhysicsEngine::setUserData(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle, const UserData& userData)
{
	auto ghostObjectData = static_cast<BulletGhostObjectData*>(ghostObjectHandle.get());
	
	ghostObjectData->userData = std::make_unique<BulletUserData>();
	
	ghostObjectData->userData->ghostObjectHandle = ghostObjectHandle;
	ghostObjectData->userData->type = BulletCollisionObjectType::GHOST;
	ghostObjectData->userData->userData = userData;
	ghostObjectData->ghostObject->setUserPointer(ghostObjectData->userData.get());
}

UserData& PhysicsEngine::getUserData(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	return rigidBodyData->userData->userData;
}

UserData& PhysicsEngine::getUserData(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle) const
{
	auto ghostObjectData = static_cast<BulletGhostObjectData*>(ghostObjectHandle.get());
	
	return ghostObjectData->userData->userData;
}

Raycast PhysicsEngine::raycast(const PhysicsSceneHandle& physicsSceneHandle, const ray::Ray& ray)
{
	Raycast result;
	result.setRay(ray);
	
	btVector3 bulletVectorStart = utilities::convert(ray.from);
	btVector3 bulletVectorEnd = utilities::convert(ray.to);
	
	btCollisionWorld::ClosestRayResultCallback rayCallback(bulletVectorStart, bulletVectorEnd);

	// Perform raycast
	physicsScenes_[physicsSceneHandle].dynamicsWorld->rayTest(bulletVectorStart, bulletVectorEnd, rayCallback);
	
	if (rayCallback.hasHit())
	{
	    result.setHitPointWorld(utilities::convert(rayCallback.m_hitPointWorld));
	    result.setHitNormalWorld(utilities::convert(rayCallback.m_hitNormalWorld));
	    
	    if (rayCallback.m_collisionObject != nullptr)
	    {
			auto bulletUserData = static_cast<BulletUserData*>(rayCallback.m_collisionObject->getUserPointer());
			
			switch (bulletUserData->type)
			{
				case BulletCollisionObjectType::RIGID_BODY:
					result.setRigidBodyObjectHandle(bulletUserData->rigidBodyObjectHandle);
					break;

				case BulletCollisionObjectType::GHOST:
					result.setGhostObjectHandle(bulletUserData->ghostObjectHandle);
					break;
				
				case BulletCollisionObjectType::UNKNOWN:
				default:
					logger_->warn("Unknown BulletCollisionObjectType");
					break;
			}
		}
	}
	
	return result;
}

void PhysicsEngine::setMotionChangeListener(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, std::unique_ptr<IMotionChangeListener> motionStateListener)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	rigidBodyData->motionState->setMotionChangeListener(std::move(motionStateListener));
}

void PhysicsEngine::rotation(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const glm::quat& orientation)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	btTransform transform = rigidBodyData->motionState->getWorldTransform();
	
	const btQuaternion quat = btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w);
	transform.setRotation(quat);
	
	rigidBodyData->rigidBody->setWorldTransform(transform);
}

glm::quat PhysicsEngine::rotation(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	const btQuaternion rotation = rigidBodyData->motionState->getWorldTransform().getRotation();
	
	return glm::quat(rotation.w(), rotation.x(), rotation.y(), rotation.z());
}

void PhysicsEngine::position(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 x, const float32 y, const float32 z)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	btTransform transform = rigidBodyData->motionState->getWorldTransform();
	
	const btVector3 pos = btVector3(x, y, z);
	transform.setOrigin(pos);
	
	rigidBodyData->rigidBody->setWorldTransform(transform);
}

void PhysicsEngine::position(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const glm::vec3& position)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());

	btTransform transform = rigidBodyData->motionState->getWorldTransform();
	
	const btVector3 pos = btVector3(position.x, position.y, position.z);
	transform.setOrigin(pos);
	
	//rigidBodyData->rigidBody->setCenterOfMassTransform(transform);
	//rigidBodyData->rigidBody->clearForces();
	//rigidBodyData->rigidBody->setLinearVelocity(btVector3(0,0,0));
	//rigidBodyData->rigidBody->setAngularVelocity(btVector3(0,0,0));
	
	rigidBodyData->rigidBody->setWorldTransform(transform);
	//rigidBodyData->rigidBody->getMotionState()->setWorldTransform(transform);
}

glm::vec3 PhysicsEngine::position(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	const btVector3 position = rigidBodyData->motionState->getWorldTransform().getOrigin();
	
	return glm::vec3(position.x(), position.y(), position.z());
}

void PhysicsEngine::position(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle, const float32 x, const float32 y, const float32 z)
{
	auto ghostObjectData = static_cast<BulletGhostObjectData*>(ghostObjectHandle.get());
	
	btTransform transform = ghostObjectData->ghostObject->getWorldTransform();
	
	const btVector3 pos = btVector3(x, y, z);
	transform.setOrigin(pos);
	
	ghostObjectData->ghostObject->setWorldTransform(transform);
}

void PhysicsEngine::position(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle, const glm::vec3& position)
{
	auto ghostObjectData = static_cast<BulletGhostObjectData*>(ghostObjectHandle.get());

	btTransform transform = ghostObjectData->ghostObject->getWorldTransform();
	
	const btVector3 pos = btVector3(position.x, position.y, position.z);
	transform.setOrigin(pos);
	
	//ghostObjectData->ghostObject->setCenterOfMassTransform(transform);
	//ghostObjectData->ghostObject->clearForces();
	//ghostObjectData->ghostObject->setLinearVelocity(btVector3(0,0,0));
	//ghostObjectData->ghostObject->setAngularVelocity(btVector3(0,0,0));
	
	ghostObjectData->ghostObject->setWorldTransform(transform);
	//ghostObjectData->ghostObject->getMotionState()->setWorldTransform(transform);
}

glm::vec3 PhysicsEngine::position(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle) const
{
	auto ghostObjectData = static_cast<BulletGhostObjectData*>(ghostObjectHandle.get());
	
	const btVector3 position = ghostObjectData->ghostObject->getWorldTransform().getOrigin();
	
	return glm::vec3(position.x(), position.y(), position.z());
}

void PhysicsEngine::mass(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 mass)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	auto shape = rigidBodyData->rigidBody->getCollisionShape();
	
	btScalar bulletMass(mass);
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	if (bulletMass != btScalar(0.0f))
	{
		shape->calculateLocalInertia(bulletMass, localInertia);
	}
	
	rigidBodyData->rigidBody->setMassProps(bulletMass, localInertia);
}

float32 PhysicsEngine::mass(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	const auto inverseMass = rigidBodyData->rigidBody->getInvMass();
	
	return pow(inverseMass, -1.0f);
}

void PhysicsEngine::friction(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 friction)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	rigidBodyData->rigidBody->setFriction(btScalar(friction));
}

float32 PhysicsEngine::friction(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	const btScalar friction = rigidBodyData->rigidBody->getFriction();
	
	return friction;
}

void PhysicsEngine::restitution(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 restitution)
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	rigidBodyData->rigidBody->setRestitution(btScalar(restitution));
}

float32 PhysicsEngine::restitution(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const
{
	auto rigidBodyData = static_cast<BulletRigidBodyData*>(rigidBodyObjectHandle.get());
	
	const btScalar restitution = rigidBodyData->rigidBody->getRestitution();
	
	return restitution;
}

}
}
}
