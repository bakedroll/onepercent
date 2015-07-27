@echo off

cd ..
call ./BUILD_DEBUG.BAT
cd ./Resources/

start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 3 0 ./boundaries/Laender.txt ./boundaries/boundries_8k.png ./TEMP/countries_map.png 2048 1024 8192 4096 ../OnePercent/GameData/data/countries.dat
start /b /w "" "..\ImageHelper\bin\x64\Debug\ImageHelper.exe" 1 1 ./TEMP/countries_map.png 2 1 ../OnePercent/GameData/textures/earth/countries/8k/