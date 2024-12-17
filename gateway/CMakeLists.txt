# Copyright (c) 2021 Nordic Semiconductor
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause

cmake_minimum_required(VERSION 3.20.0)

# Find Zephyr
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(gateway)

# Add application source directory
target_sources(app PRIVATE src/main.c)

# Include directories
zephyr_include_directories(src)
zephyr_include_directories(config)

# Define Memfault custom metrics
set(MEMFAULT_METRICS_USER_HEARTBEAT_DEFS_FILE "memfault_metrics_heartbeat_zephyr_port_config.def")
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/config)

# Link Memfault SDK library
target_link_libraries(app PRIVATE memfault-firmware-sdk)
