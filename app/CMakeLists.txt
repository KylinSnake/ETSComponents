add_executable(snake
	snake.cpp
)
target_link_libraries(snake core)

install(TARGETS snake
		RUNTIME DESTINATION bin)

add_library(test_main OBJECT test_main.cpp)
target_link_libraries(test_main PRIVATE tp::catch2)
