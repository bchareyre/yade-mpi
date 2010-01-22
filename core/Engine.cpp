#include<yade/core/Engine.hpp>
#include<yade/core/Scene.hpp>
void Engine::explicitAction(){
	scene=Omega::instance().getScene().get();
	this->action(scene);
}

