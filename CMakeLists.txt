#
# LED Demo
#
# Copyright (C) 2019-2024, HENSOLDT Cyber GmbH
# 
# SPDX-License-Identifier: GPL-2.0-or-later
#
# For commercial licensing, contact: info.cyber@hensoldt.net
#

cmake_minimum_required(VERSION 3.7.2)

os_set_config_file(
    system_config.h
    CONFIG_PROJECT_NAME system_config)

project(demo_led_app C)

set(LibUtilsDefaultZfLogLevel 5 CACHE STRING "" FORCE)

DeclareCAmkESComponent(
    Client
    SOURCES
        Client/client.c
    LIBS
        os_core_api
)

RPi_SPI_LED_DeclareCAmkESComponent(
    LED
    LIBS
        TimeServer_client
)

TimeServer_DeclareCAmkESComponent(
    TimeServer
)

DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
