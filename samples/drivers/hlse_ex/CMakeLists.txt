cmake_minimum_required(VERSION 3.13.1)
include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(hlse_ex)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})

add_definitions(
	-DTGT_A71CH
	-DI2C
	-DNO_SECURE_CHANNEL_SUPPORT
	)
