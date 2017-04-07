#ifndef IGRAPHICSENGINE_H_
#define IGRAPHICSENGINE_H_

#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "Types.hpp"

#include "IEventListener.hpp"
#include "RenderableHandle.hpp"
#include "MeshHandle.hpp"
#include "TextureHandle.hpp"
#include "SkeletonHandle.hpp"
#include "CameraHandle.hpp"

#include "model/Model.hpp"

namespace hercules
{
namespace graphics
{

enum TransformSpace
{
	TS_LOCAL = 0,
	TS_WORLD
};

class IGraphicsEngine
{
public:
	virtual ~IGraphicsEngine()
	{
	}
	;
	
	virtual void setViewport(const uint32 width, const uint32 height) = 0;
	virtual void render(const float32 delta) = 0;
	
	virtual CameraHandle createCamera(const glm::vec3& position, const glm::vec3& lookAt = glm::vec3(0.0f, 0.0f, 0.0f)) = 0;
	
	virtual MeshHandle createStaticMesh(
		const std::vector<glm::vec3>& vertices,
		const std::vector<uint32>& indices,
		const std::vector<glm::vec4>& colors,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& textureCoordinates
	) = 0;
	virtual MeshHandle createAnimatedMesh(
		const std::vector<glm::vec3>& vertices,
		const std::vector<uint32>& indices,
		const std::vector<glm::vec4>& colors,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& textureCoordinates,
		const std::vector<glm::ivec4>& boneIds,
		const std::vector<glm::vec4>& boneWeights
	) = 0;
	virtual MeshHandle createDynamicMesh(
		const std::vector<glm::vec3>& vertices,
		const std::vector<uint32>& indices,
		const std::vector<glm::vec4>& colors,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& textureCoordinates
	) = 0;
	
	virtual SkeletonHandle createSkeleton(const uint32 numberOfBones) = 0;
	
	virtual TextureHandle createTexture2d(const utilities::Image& image) = 0;
	
	virtual RenderableHandle createRenderable(const MeshHandle& meshHandle, const TextureHandle& textureHandle) = 0;
	
	virtual void rotate(const CameraHandle& cameraHandle, const glm::quat& quaternion, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) = 0;
	virtual void rotate(const RenderableHandle& renderableHandle, const glm::quat& quaternion, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) = 0;
	virtual void rotate(const CameraHandle& cameraHandle, const float32 degrees, const glm::vec3& axis, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) = 0;
	virtual void rotate(const RenderableHandle& renderableHandle, const float32 degrees, const glm::vec3& axis, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) = 0;
	
	virtual void translate(const CameraHandle& cameraHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void translate(const RenderableHandle& renderableHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void translate(const CameraHandle& cameraHandle, const glm::vec3& trans) = 0;
	virtual void translate(const RenderableHandle& renderableHandle, const glm::vec3& trans) = 0;
	
	virtual void scale(const RenderableHandle& renderableHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void scale(const RenderableHandle& renderableHandle, const glm::vec3& scale) = 0;
	virtual void scale(const RenderableHandle& renderableHandle, const float32 scale) = 0;
	
	virtual void position(const RenderableHandle& renderableHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void position(const CameraHandle& cameraHandle, const float32 x, const float32 y, const float32 z) = 0;
	virtual void position(const RenderableHandle& renderableHandle, const glm::vec3& position) = 0;
	virtual void position(const CameraHandle& cameraHandle, const glm::vec3& position) = 0;
	
	virtual void lookAt(const RenderableHandle& renderableHandle, const glm::vec3& lookAt) = 0;
	virtual void lookAt(const CameraHandle& cameraHandle, const glm::vec3& lookAt) = 0;
	
	virtual void assign(const RenderableHandle& renderableHandle, const SkeletonHandle& skeletonHandle) = 0;
	
	virtual void update(const SkeletonHandle& skeletonHandle, const void* data, const uint32 size) = 0;
	
	virtual void setMouseRelativeMode(const bool enabled) = 0;
	virtual void setCursorVisible(const bool visible) = 0;
	
	virtual void processEvents() = 0;
	virtual void addEventListener(IEventListener* eventListener) = 0;
	virtual void removeEventListener(IEventListener* eventListener) = 0;
};

}

}

#endif /* IGRAPHICSENGINE_H_ */
