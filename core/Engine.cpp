#include<yade/core/Engine.hpp>

CREATE_LOGGER(Engine);

void Engine::action(){
	LOG_FATAL("Engine "<<getClassName()<<" calling virtual method Engine::action(). Please submit bug report at http://bugs.launchpad.net/yade.");
	throw std::logic_error("Engine::action() called.");
}

