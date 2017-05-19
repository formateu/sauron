find_package(Boost COMPONENTS REQUIRED unit_test_framework thread system)

add_subdirectory(common)
add_subdirectory(unit_tests)
add_subdirectory(integration_tests)
