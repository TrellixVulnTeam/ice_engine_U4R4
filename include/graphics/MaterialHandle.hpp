#ifndef MATERIAL_HANDLE_H_
#define MATERIAL_HANDLE_H_

#include "ResourceHandle.hpp"

namespace hercules
{
namespace graphics
{

class MaterialHandle : public ResourceHandle
{
public:
	using ResourceHandle::ResourceHandle;
	
	static const MaterialHandle INVALID;
};

}
}

#endif /* MATERIAL_HANDLE_H_ */