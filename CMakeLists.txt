cmake_minimum_required(VERSION 3.5)
project(tieto_proc)

add_subdirectory (src) 
add_subdirectory (tests) 

target_compile_options(app PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

enable_testing()

add_test(NAME test1 COMMAND test_exec1)
