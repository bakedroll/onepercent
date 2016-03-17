@echo off

cd ..
call ./BUILD_DEBUG.BAT
cd ./Resources/

start /b /w "" "..\Debug\ImageHelperCpp.exe" -i ./earth_textures/boundaries.png -p ./TEMP/countries -t .\triangle\bin\triangle.exe -D ./TEMP/dbg -d 10 -a 15 -r 0.7 -o ../OnePercent/GameData/data/boundaries.dat