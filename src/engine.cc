#include "engine.h"

#define DEBUG

Engine :: Engine (Loader * loader)
{
	this->loader = loader;
	vms.push_back(new VM(loader, this));
}

Engine :: ~Engine ()
{
	std::list <VM *> :: iterator it;
	for (it = vms.begin(); it != vms.end(); it++) {
		delete *it;
	}
}

void Engine :: step ()
{
	std::list <VM *> :: iterator it;

	for (it = vms.begin(); it != vms.end(); it++) {
		(*it)->step();
	}
}

void Engine :: push_vm (VM * vm)
{
	#ifdef DEBUG
	std::cout << "push_vm, vm count = " << vms.size() << std::endl;
	#endif
	vms.push_back(vm);
}

bool Engine :: remove_vm (VM * vm)
{
	std::list <VM *> :: iterator it;

	for (it = vms.begin(); it != vms.end(); it++) {
		if (*it == vm) {
			vms.erase(it);
			return true;
		}
	}

	return false;
}

size_t Engine :: g_size ()
{
	return vms.size();
}