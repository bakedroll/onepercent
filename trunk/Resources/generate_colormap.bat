@echo off

cd ..
call ./BUILD_DEBUG.BAT
cd ./Resources/

start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 4 0 ./earth_textures/4_no_ice_clouds_mts_8k.jpg ./earth_textures/water_8k.png ./earth_textures/relief_8k.png ./TEMP/colormap.png
start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 1 0 ./TEMP/colormap.png 2 1 ../OnePercent/GameData/textures/earth/color/8k/