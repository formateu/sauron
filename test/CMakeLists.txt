find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_subdirectory(common)
add_subdirectory(unit_tests)
add_subdirectory(integration_tests)
