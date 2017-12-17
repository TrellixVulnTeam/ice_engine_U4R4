#include "IceEngineMotionChangeListener.hpp"

namespace ice_engine
{

IceEngineMotionChangeListener::IceEngineMotionChangeListener(entities::Entity entity, IScene* scene) : entity_(entity), scene_(scene)
{
	
}

IceEngineMotionChangeListener::~IceEngineMotionChangeListener()
{
}

void IceEngineMotionChangeListener::update(const glm::vec3& position, const glm::quat& orientation)
{
	scene_->rotation(entity_, orientation);
	scene_->position(entity_, position);
}


}