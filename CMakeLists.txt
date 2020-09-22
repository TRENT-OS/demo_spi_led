#
# LED Demo
#
# Copyright (C) 2019-2020, Hensoldt Cyber GmbH
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

DeclareCAmkESComponent_RPI_SPI_LED(
    LED
    LIBS
        TimeServer_client
)

DeclareCAmkESComponent_TimeServer(
    TimeServer
)

DeclareAndCreateCamkESSystem(main.camkes)
GenerateSimulateScript()
