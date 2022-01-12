py bin/sigthief.py -i bin/clean.exe -t in.exe -o good.exe
start bin/upx.exe good.exe
Del in.exe