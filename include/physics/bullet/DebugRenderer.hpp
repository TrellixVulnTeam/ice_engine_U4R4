#ifndef PHYSICSDEBUGRENDERER_H_
#define PHYSICSDEBUGRENDERER_H_

#include <bullet/LinearMath/btIDebugDraw.h>

#include "physics/IPhysicsDebugRenderer.hpp"

namespace hercules
{
namespace physics
{
namespace bullet
{

class DebugRenderer : public btIDebugDraw
{
public:
	DebugRenderer(physics::IPhysicsDebugRenderer* physicsDebugRenderer) : physicsDebugRenderer_(physicsDebugRenderer)
	{
	}
	
	virtual ~DebugRenderer()
	{
	}
	
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override
	{
		physicsDebugRenderer_->pushLine(glm::vec3(from.x(), from.y(), from.z()), glm::vec3(to.x(), to.y(), to.z()), glm::vec3(color.x(), color.y(), color.z()));
	};
	
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override {};
	virtual void reportErrorWarning(const char* warningString) override {};
	virtual void draw3dText(const btVector3& location, const char* textString) override {};
	virtual void setDebugMode(int debugMode) override
	{
		debugDrawMode_ = debugMode;
	};
	
	virtual int getDebugMode() const override
	{
		return debugDrawMode_;
	};
	
private:
	IPhysicsDebugRenderer* physicsDebugRenderer_ = nullptr;
	int debugDrawMode_ =  DBG_DrawWireframe | DBG_DrawAabb;

};

}
}
}

#endif /* PHYSICSDEBUGRENDERER_H_ */