#!/bin/bash
USERNAME="louis chabanon"
read -r -p "enter your username (15 caracteres max) " USERNAME
#clear
echo "1) omega_light          2) omega_dark"
echo "3) epsilon_light        4) epsilon_dard"
read -r -p "choose the theme you wish to install: " key
case $key in
	1) THEME="OMEGA_LIGHT" ;;
	2) THEME="OMEGA_DARK" ;;
	3) THEME="EPSILON_LIGHT" ;;
	4) THEME="EPSILON_DARK" ;;
	*) echo "you must enter a valid input (1,2,3,4)" && exit
esac
make clean
make OMEGA_USERNAME="$USERNAME" THEME_NAME="$THEME" -j4
echo ""
echo "Please make sure your numworks is connected"
sleep 2
make epsilon_flash

