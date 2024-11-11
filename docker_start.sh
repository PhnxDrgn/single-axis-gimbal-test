#!/bin/bash

dir_name=${PWD##*/}

docker compose run --rm -d --name ${dir_name} cmake-on-stm32