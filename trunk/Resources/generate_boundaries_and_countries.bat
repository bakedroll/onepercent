@echo off

cd ..
call ./BUILD_DEBUG.BAT
cd ./Resources/

start /b /w "" "..\Debug\ImageHelperCpp.exe" -i ../Resources/earth_textures/boundaries.png -p ../Resources/TEMP/countries -t ..\Resources\triangle\bin\triangle.exe -D ../Resources/TEMP/dbg -d 10 -a 20 -r 0.6 -o ./GameData/data/boundaries.dat -c ./boundaries/Laender.txt -co ./GameData/data/countries.dat