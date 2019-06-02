snake_lib(core
	src/Log.cpp
	src/Logger.cpp
	src/Application.cpp
	src/Main.cpp
	src/ServiceFactory.cpp
	src/Service.cpp
	src/Config.cpp
)

snake_link(core PUBLIC util PUBLIC concurrency PRIVATE ${CMAKE_DL_LIBS} PUBLIC yaml-cpp)


snake_lib(test_service test/fake_service.cpp)
snake_link(test_service PRIVATE core)

snake_test(core
	test/services_test.cpp
)
snake_test_link(core)
