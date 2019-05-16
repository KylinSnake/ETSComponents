add_executable(snake
	snake.cpp
)
target_link_libraries(snake core)

install(TARGETS snake
		RUNTIME DESTINATION bin)
