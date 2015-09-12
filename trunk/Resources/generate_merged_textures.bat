@echo off

cd ..
call ./BUILD_DEBUG.BAT
cd ./Resources/

start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 0 0 ./earth_textures/water_8k.png ./earth_textures/relief_8k.png ./earth_textures/cities_8k.png ./earth_textures/boundaries_8k.png ./TEMP/specreliefcitiesbounds.png
start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 1 0 ./TEMP/specreliefcitiesbounds.png 2 1 ../OnePercent/GameData/textures/earth/specreliefcitiesbounds/8k/