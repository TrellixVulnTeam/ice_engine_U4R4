#ifndef BULLETPHYSICSUTILITIES_H_
#define BULLETPHYSICSUTILITIES_H_

#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btQuaternion.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace hercules
{
namespace physics
{
namespace bullet
{
namespace utilities
{

// Bullet to GLM
inline glm::vec3 convert(const btVector3& bulletVector3)
{
	return glm::vec3(bulletVector3.x(), bulletVector3.y(), bulletVector3.z());
};

inline glm::quat convert(const btQuaternion& bulletQuaternion)
{
	return glm::quat(bulletQuaternion.w(), bulletQuaternion.x(), bulletQuaternion.y(), bulletQuaternion.z());
};

// GLM to Bullet
inline btVector3 convert(const glm::vec3& glmVector3)
{
	return btVector3(glmVector3.x, glmVector3.y, glmVector3.z);
};

inline btQuaternion convert(const glm::quat& glmQuat)
{
	return btQuaternion(glmQuat.x, glmQuat.y, glmQuat.z, glmQuat.w);
};

}
}
}
}

#endif /* BULLETPHYSICSUTILITIES_H_ */