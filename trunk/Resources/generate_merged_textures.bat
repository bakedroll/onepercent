@echo off

cd ..
call ./BUILD_DEBUG.BAT
cd ./Resources/

start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 0 0 ./earth_textures/water_8k.png ./earth_textures/cities_8k.png ./earth_textures/clouds/fair_clouds_8k.png ./earth_textures/clouds/se_asia_clouds_8k.png ./TEMP/speccitiesclouds.png
start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 1 0 ./TEMP/speccitiesclouds.png 2 1 ../OnePercent/GameData/textures/earth/speccitiesclouds/8k/