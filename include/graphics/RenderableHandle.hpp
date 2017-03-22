#ifndef RENDERABLE_HANDLE_H_
#define RENDERABLE_HANDLE_H_

#include "ResourceHandle.hpp"

namespace hercules
{
namespace graphics
{

class RenderableHandle : public ResourceHandle
{
public:
	using ResourceHandle::ResourceHandle;
	
	static const RenderableHandle INVALID;
};

}
}

#endif /* RENDERABLE_HANDLE_H_ */