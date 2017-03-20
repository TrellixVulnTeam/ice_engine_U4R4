#ifndef GRAPHICSENGINE_H_
#define GRAPHICSENGINE_H_

#include <string>
#include <vector>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <bgfx/platform.h>
#include <bgfx/bgfx.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "graphics/IGraphicsEngine.hpp"
#include "graphics/ShaderHandle.hpp"
#include "graphics/Event.hpp"

#include "utilities/Properties.hpp"
#include "fs/IFileSystem.hpp"
#include "logger/ILogger.hpp"

namespace hercules
{
namespace graphics
{
namespace bgfx
{

struct Vbo
{
	GLuint id;
};

struct Ebo
{
	GLuint id;
	GLenum mode;
  	GLsizei count;
	GLenum type;
};

struct Ubo
{
	GLuint id;
};

struct Vao
{
	GLuint id;
	::bgfx::VertexBufferHandle vbh;
	::bgfx::IndexBufferHandle ibh;
};

struct GlTexture2d
{
	GLuint id;
};

struct Renderable
{
	Vao vao;
	Ubo ubo;
	GlTexture2d texture;
};


struct GraphicsData
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat orientation;
};

struct Camera
{
	glm::vec3 position;
	glm::quat orientation;
};

class GraphicsEngine : public IGraphicsEngine
{
public:
	GraphicsEngine(utilities::Properties* properties, fs::IFileSystem* fileSystem, logger::ILogger* logger);
	virtual ~GraphicsEngine();
	
	virtual void setViewport(const uint32 width, const uint32 height) override;
	virtual void render(const float32 delta) override;
	
	virtual CameraHandle createCamera(const glm::vec3& position, const glm::vec3& lookAt = glm::vec3(0.0f, 0.0f, 0.0f)) override;
	
	virtual MeshHandle createStaticMesh(
		const std::vector<glm::vec3>& vertices,
		const std::vector<uint32>& indices,
		const std::vector<glm::vec4>& colors,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& textureCoordinates
	) override;
	virtual MeshHandle createAnimatedMesh(
		const std::vector<glm::vec3>& vertices,
		const std::vector<uint32>& indices,
		const std::vector<glm::vec4>& colors,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& textureCoordinates,
		const std::vector<glm::ivec4>& boneIds,
		const std::vector<glm::vec4>& boneWeights
	) override;
	virtual MeshHandle createDynamicMesh(
		const std::vector<glm::vec3>& vertices,
		const std::vector<uint32>& indices,
		const std::vector<glm::vec4>& colors,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& textureCoordinates
	) override;
	
	virtual SkeletonHandle createSkeleton(const uint32 numberOfBones) override;
	
	virtual TextureHandle createTexture2d(const std::string& uri) override;
	
	virtual RenderableHandle createRenderable(const MeshHandle meshHandle, const TextureHandle textureHandle) override;
	
	virtual void rotate(const CameraHandle cameraHandle, const glm::quat& quaternion, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) override;
	virtual void rotate(const RenderableHandle renderableHandle, const glm::quat& quaternion, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) override;
	virtual void rotate(const CameraHandle cameraHandle, const float32 degrees, const glm::vec3& axis, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) override;
	virtual void rotate(const RenderableHandle renderableHandle, const float32 degrees, const glm::vec3& axis, const TransformSpace& relativeTo = TransformSpace::TS_LOCAL) override;
	
	virtual void translate(const CameraHandle cameraHandle, const float32 x, const float32 y, const float32 z) override;
	virtual void translate(const RenderableHandle renderableHandle, const float32 x, const float32 y, const float32 z) override;
	virtual void translate(const CameraHandle cameraHandle, const glm::vec3& trans) override;
	virtual void translate(const RenderableHandle renderableHandle, const glm::vec3& trans) override;
	
	virtual void scale(const RenderableHandle renderableHandle, const float32 x, const float32 y, const float32 z) override;
	virtual void scale(const RenderableHandle renderableHandle, const glm::vec3& scale) override;
	virtual void scale(const RenderableHandle renderableHandle, const float32 scale) override;
	
	virtual void position(const RenderableHandle renderableHandle, const float32 x, const float32 y, const float32 z) override;
	virtual void position(const CameraHandle cameraHandle, const float32 x, const float32 y, const float32 z) override;
	virtual void position(const RenderableHandle renderableHandle, const glm::vec3& position) override;
	virtual void position(const CameraHandle cameraHandle, const glm::vec3& position) override;
	
	virtual void lookAt(const RenderableHandle renderableHandle, const glm::vec3& lookAt) override;
	virtual void lookAt(const CameraHandle cameraHandle, const glm::vec3& lookAt) override;
	
	virtual void assign(const RenderableHandle renderableHandle, const SkeletonHandle skeletonHandle) override;
	
	virtual void update(const SkeletonHandle skeletonHandle, const void* data, const uint32 size) override;
	
	virtual void setMouseRelativeMode(const bool enabled) override;
	virtual void setCursorVisible(const bool visible) override;
	
	virtual void processEvents() override;
	virtual void addEventListener(IEventListener* eventListener) override;
	virtual void removeEventListener(IEventListener* eventListener) override;

private:
	GraphicsEngine(const GraphicsEngine& other);
	
	uint32 width_;
	uint32 height_;
	
	::bgfx::ProgramHandle shaderProgram_;
	
	SDL_Window* sdlWindow_;
	
	std::vector<IEventListener*> eventListeners_;
	std::vector<Renderable> renderables_;
	std::vector<Vao> vertexArrayObjects_;
	std::vector<Ubo> uniformBufferObjects_;
	std::vector<GlTexture2d> texture2dObjects_;
	Camera camera_;
	
	std::vector<GraphicsData> graphicsData_;
	glm::mat4 model_;
	glm::mat4 view_;
	glm::mat4 projection_;
	
	utilities::Properties* properties_;
	fs::IFileSystem* fileSystem_;
	logger::ILogger* logger_;
	
	::bgfx::ProgramHandle createShaderProgram(const std::string& vertexShaderUri, const std::string& fragmentShaderUri);
	::bgfx::ProgramHandle createShaderProgramFromSource(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	
	std::string compileShaderToFile(const std::string& shaderFile, const ShaderType shaderType, const std::vector<std::string>& includeDirectories = std::vector<std::string>()) const;
	
	std::string getShaderErrorMessage(const GLuint shader);
	std::string getShaderProgramErrorMessage(const GLuint shaderProgram);
	
	void handleEvent(const Event& event);
	static Event convertSdlEvent(const SDL_Event& event);
	static KeySym convertSdlKeySym(const SDL_Keysym& keySym);
	static ScanCode convertSdlScancode(const SDL_Scancode& sdlScancode);
	static KeyCode convertSdlKeycode(const SDL_Keycode& sdlKeycode);
};

}
}
}

#endif /* GRAPHICSENGINE_H_ */
