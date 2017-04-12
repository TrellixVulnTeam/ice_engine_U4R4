#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <string>

#include "utilities/Image.hpp"

//#include "glw/TextureSettings.hpp"
//#include "graphics/model/VertexBoneData.hpp"

namespace hercules
{
namespace graphics
{
namespace model
{

struct Texture
{
	std::string filename;
	utilities::Image image;
	//glw::TextureSettings settings;
};

}
}
}

#endif /* TEXTURE_H_ */