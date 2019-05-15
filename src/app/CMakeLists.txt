add_executable(snake
	snake.cpp
)
target_link_libraries(snake core)

install(TARGETS snake
		CONFIGURATIONS Debug
		RUNTIME DESTINATION bin/Debug)
install(TARGETS snake
		CONFIGURATIONS Release
		RUNTIME DESTINATION bin/Release)
