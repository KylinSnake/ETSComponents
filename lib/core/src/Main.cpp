#include <iostream>

#include <snake/core/Main.h>
#include <snake/core/Log.h>
#include <Application.hpp>
#include <snake/util/Exception.h>

namespace snake
{
	using Application = snake::Application;
	using Service = snake::Service;
	using Exception = snake::Exception;

	inline Application& app()
	{
		static Application app;
		return app;
	}

	void Main::shutdown()
	{
		app().shutdown();
	}

	ServicePointer Main::get_service_ptr(const std::string& name)
	{
		return app().get_service_pointer(name);
	}

	int Main::main(int argc, char* argv[])
	{
		if(argc <= 1)
		{
			std::cerr << "\tusage: " << argv[0] << " <yaml-config-file>" << std::endl;
			return -1;
		}

		std::string file(argv[1]);

		if(app().load_config_file(file))
		{
			return -1;
		}

		auto root  = app().root();
		if(!app().initialize_log(root))
		{
			return -1;
		}

		auto nodes = root.get_child("Service").get_node_list_of_children();

		if(nodes.size() == 0)
		{
			LOG_FATAL << "No any service_configured" << ENDLOG;
			return -1;
		}

		int ret = 0;
		try
		{
			if(app().initialize_services(nodes))
			{
				app().start_services();
				app().run();
			}
		}
		catch(const Exception& e)
		{
			ret = -1;
			LOG_ERROR << "Exception catched in main thread, shutdown system. [error = " << e.what() << "]" << ENDLOG
		}
        
		LOG_INFO << "Shut down the system" << ENDLOG;
		app().stop_services();
		app().join_services();

		return ret;
	}
}
