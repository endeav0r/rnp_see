#ifndef engine_HEADER
#define engine_HEADER

class Engine;

#include "loader.h"
#include "vm.h"

#include <list>

class Engine {
	private :
		Loader * loader;
		std::list <VM *> vms;
	public :
		Engine  (Loader * loader);
		~Engine ();

		void step ();

		void push_vm   (VM * vm);
		bool remove_vm (VM * vm);

		size_t g_size ();
};

#endif