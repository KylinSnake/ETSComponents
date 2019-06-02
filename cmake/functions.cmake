function(snake_lib target_name)
	add_library(${target_name} ${ARGN})
	
	target_include_directories(${target_name}
		PUBLIC
			$<INSTALL_INTERFACE:${INSTALL_HEADER_FOLDER}>
			$<BUILD_INTERFACE:${PUBLIC_HEADER_FOLDER}>
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	if(TEST_COVERAGE)
		target_compile_options(${target_name} PRIVATE "--coverage")
		target_link_options(${target_name} PRIVATE "--coverage")
	endif()

	install(TARGETS ${target_name}
			EXPORT snakes-exports
			LIBRARY DESTINATION lib
			ARCHIVE DESTINATION lib)
endfunction()

function(snake_link target_name)
	target_link_libraries(${target_name} ${ARGN})
endfunction()

function(snake_test target)
	set(test_target "${target}_test")
	add_executable(${test_target} 
		$<TARGET_OBJECTS:test_main>
		${ARGN}
	)
	target_include_directories(${test_target}
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/src
	)

	if(TEST_COVERAGE)
		target_compile_options(${test_target} PRIVATE "--coverage")
		target_link_options(${test_target} PRIVATE "--coverage")
	endif()

	add_test(NAME ${test_target} COMMAND $<TARGET_FILE:${test_target}>)
	set_tests_properties(${test_target} PROPERTIES DEPENDS ${target})
endfunction()

function(snake_test_link target)
	set(test_target "${target}_test")
	target_link_libraries(${test_target} PRIVATE ${target} PRIVATE tp::catch2 ${ARGN})
endfunction()

