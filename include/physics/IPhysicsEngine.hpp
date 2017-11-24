#ifndef IPHYSICSENGINE_H_
#define IPHYSICSENGINE_H_

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Types.hpp"

#include "ray/Ray.hpp"

#include "physics/PhysicsSceneHandle.hpp"
#include "physics/CollisionShapeHandle.hpp"
#include "physics/RigidBodyObjectHandle.hpp"
#include "physics/GhostObjectHandle.hpp"
#include "physics/IMotionChangeListener.hpp"
#include "physics/IPhysicsDebugRenderer.hpp"
#include "physics/UserData.hpp"
#include "physics/Raycast.hpp"

namespace hercules
{
namespace physics
{

class IPhysicsEngine
{
public:
	virtual ~IPhysicsEngine()
	{
	}
	;
	
	virtual void tick(const PhysicsSceneHandle& physicsSceneHandle, const float32 delta) = 0;
	
	virtual PhysicsSceneHandle createPhysicsScene() = 0;
	virtual void destroyPhysicsScene(const PhysicsSceneHandle& physicsSceneHandle) = 0;
	
	virtual void setGravity(const PhysicsSceneHandle& physicsSceneHandle, const glm::vec3& gravity) = 0;
	
	virtual void setPhysicsDebugRenderer(IPhysicsDebugRenderer* physicsDebugRenderer) = 0;
	
	virtual CollisionShapeHandle createStaticPlaneShape(const glm::vec3& planeNormal, const float32 planeConstant) = 0;
	virtual CollisionShapeHandle createStaticBoxShape(const glm::vec3& dimensions) = 0;
	virtual void destroyStaticShape(const CollisionShapeHandle& collisionShapeHandle) = 0;
	virtual void destroyAllStaticShapes() = 0;
	
	virtual RigidBodyObjectHandle createDynamicRigidBodyObject(
		const PhysicsSceneHandle& physicsSceneHandle, 
		const CollisionShapeHandle& collisionShapeHandle,
		std::unique_ptr<IMotionChangeListener> motionStateListener = nullptr,
		const UserData& userData = UserData()
	) = 0;
	virtual RigidBodyObjectHandle createDynamicRigidBodyObject(
		const PhysicsSceneHandle& physicsSceneHandle, 
		const CollisionShapeHandle& collisionShapeHandle,
		const float32 mass,
		const float32 friction,
		const float32 restitution,
		std::unique_ptr<IMotionChangeListener> motionStateListener = nullptr,
		const UserData& userData = UserData()
	) = 0;
	virtual RigidBodyObjectHandle createDynamicRigidBodyObject(
		const PhysicsSceneHandle& physicsSceneHandle, 
		const CollisionShapeHandle& collisionShapeHandle,
		const glm::vec3& position,
		const glm::quat& orientation,
		const float32 mass = 1.0f,
		const float32 friction = 1.0f,
		const float32 restitution = 1.0f,
		std::unique_ptr<IMotionChangeListener> motionStateListener = nullptr,
		const UserData& userData = UserData()
	) = 0;
	virtual RigidBodyObjectHandle createStaticRigidBodyObject(const PhysicsSceneHandle& physicsSceneHandle, const CollisionShapeHandle& collisionShapeHandle, const UserData& userData = UserData()) = 0;
	virtual RigidBodyObjectHandle createStaticRigidBodyObject(
		const PhysicsSceneHandle& physicsSceneHandle, 
		const CollisionShapeHandle& collisionShapeHandle,
		const float32 friction,
		const float32 restitution,
		const UserData& userData = UserData()
	) = 0;
	virtual RigidBodyObjectHandle createStaticRigidBodyObject(
		const PhysicsSceneHandle& physicsSceneHandle, 
		const CollisionShapeHandle& collisionShapeHandle,
		const glm::vec3& position,
		const glm::quat& orientation,
		const float32 friction = 1.0f,
		const float32 restitution = 1.0f,
		const UserData& userData = UserData()
	) = 0;
	virtual GhostObjectHandle createGhostObject(const PhysicsSceneHandle& physicsSceneHandle, const CollisionShapeHandle& collisionShapeHandle, const UserData& userData = UserData()) = 0;
	virtual GhostObjectHandle createGhostObject(
		const PhysicsSceneHandle& physicsSceneHandle, 
		const CollisionShapeHandle& collisionShapeHandle,
		const glm::vec3& position,
		const glm::quat& orientation,
		const UserData& userData = UserData()
	) = 0;
	virtual void destroyRigidBody(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) = 0;
	virtual void destroyAllRigidBodies() = 0;
	
	virtual void setUserData(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const UserData& userData) = 0;
	virtual void setUserData(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle, const UserData& userData) = 0;
	virtual UserData& getUserData(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const = 0;
	virtual UserData& getUserData(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle) const = 0;
	
	virtual Raycast raycast(const PhysicsSceneHandle& physicsSceneHandle, const ray::Ray& ray) = 0;
	
	virtual void setMotionChangeListener(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, std::unique_ptr<IMotionChangeListener> motionStateListener) = 0;
	
	virtual void rotation(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const glm::quat& orientation) = 0;
	virtual glm::quat rotation(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const = 0;
	
	virtual void position(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void position(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const glm::vec3& position) = 0;
	virtual glm::vec3 position(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const = 0;
	
	virtual void position(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void position(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle, const glm::vec3& position) = 0;
	virtual glm::vec3 position(const PhysicsSceneHandle& physicsSceneHandle, const GhostObjectHandle& ghostObjectHandle) const = 0;
	
	virtual void mass(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 mass) = 0;
	/**
	 * Note: This value must be calculated, so it may not be exactly what you set originally.
	 */
	virtual float32 mass(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const = 0;
	
	virtual void friction(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 friction) = 0;
	virtual float32 friction(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const = 0;
	
	virtual void restitution(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle, const float32 restitution) = 0;
	virtual float32 restitution(const PhysicsSceneHandle& physicsSceneHandle, const RigidBodyObjectHandle& rigidBodyObjectHandle) const = 0;
};

}
}

#endif /* IPHYSICSENGINE_H_ */
