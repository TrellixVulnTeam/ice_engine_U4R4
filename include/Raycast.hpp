#ifndef RAYCAST_H_
#define RAYCAST_H_

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "entities/Entity.hpp"

#include "ray/Ray.hpp"

#include "Types.hpp"

namespace hercules
{

class Raycast
{
public:
	Raycast() = default;

	Raycast(const ray::Ray& ray) : ray_(ray)
	{
	}

	const ray::Ray& ray() const
	{
		return ray_;
	}

	void setRay(const ray::Ray& ray)
	{
		ray_ = ray;
	}

	const glm::vec3& hitPointWorld() const
	{
		return hitPointWorld_;
	}

	void setHitPointWorld(const glm::vec3& hitPointWorld)
	{
		hitPointWorld_ = hitPointWorld;
	}

	const glm::vec3& hitNormalWorld() const
	{
		return hitNormalWorld_;
	}

	void setHitNormalWorld(const glm::vec3& hitNormalWorld)
	{
		hitNormalWorld_ = hitNormalWorld;
	}

	const entities::Entity& entity() const
	{
		return entity_;
	}

	void setEntity(const entities::Entity& entity)
	{
		entity_ = entity;
	}

protected:
	//~Raycast() = default;

private:
	ray::Ray ray_;
	glm::vec3 hitPointWorld_;
	glm::vec3 hitNormalWorld_;
	entities::Entity entity_;
};

}

#endif /* RAYCAST_H_ */

