#!/bin/bash
USERNAME="louis chabanon"
read -r -p "enter your username (15 caracteres max) " USERNAME
make clean
make OMEGA_USERNAME="$USERNAME" THEME_NAME=EPSILON_LIGHT EPSILON_I18N=fr -j4
echo ""
echo "Please make sure your numworks is connected"
sleep 2
make epsilon_flash

