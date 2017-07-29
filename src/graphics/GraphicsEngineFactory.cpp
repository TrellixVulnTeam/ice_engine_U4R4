#include "graphics/GraphicsEngineFactory.hpp"
#include "graphics/custom/GraphicsEngine.hpp"

namespace hercules
{
namespace graphics
{

GraphicsEngineFactory::GraphicsEngineFactory()
{
}

GraphicsEngineFactory::GraphicsEngineFactory(const GraphicsEngineFactory& other)
{
}

GraphicsEngineFactory::~GraphicsEngineFactory()
{
}

std::unique_ptr<IGraphicsEngine> GraphicsEngineFactory::create(
	utilities::Properties* properties,
	fs::IFileSystem* fileSystem,
	logger::ILogger* logger
)
{
	std::unique_ptr<IGraphicsEngine> ptr = std::make_unique< custom::GraphicsEngine >( properties, fileSystem, logger );
	
	return std::move( ptr );
}

}
}
