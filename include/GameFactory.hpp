#ifndef GAMEFACTORY_H_
#define GAMEFACTORY_H_

#include <memory>

#include "IGameEngine.hpp"

namespace hercules
{

class GameFactory
{
public:

	static std::unique_ptr<IGameEngine> createGameEngine();

private:
	GameFactory();
	GameFactory(const GameFactory& other);
	virtual ~GameFactory();
};

}

#endif /* GAMEFACTORY_H_ */