./bin/Linux/main: src/main.cpp src/glad.c src/textrendering.cpp include/matrices.h include/utils.h include/dejavufont.h
	g++.exe -Wall -Wno-unused-function -Wall -std=c++11 -g -Iinclude -c src\main.cpp -o obj\Debug\src\main.o
	g++.exe -Wall -Wno-unused-function -Wall -std=c++11 -g -Iinclude -c src\collisions.cpp -o obj\Debug\src\collisions.o
	g++.exe -Wall -Wno-unused-function -Wall -std=c++11 -g -Iinclude -c src\textrendering.cpp -o obj\Debug\src\textrendering.o
	g++.exe -Llib -o bin\Debug\main.exe obj\Debug\src\glad.o obj\Debug\src\main.o obj\Debug\src\collisions.o obj\Debug\src\textrendering.o obj\Debug\src\tiny_obj_loader.o  lib\libglfw3.a -lgdi32 -lopengl32 -static-libstdc++ -static-libgcc -static  
	cd bin\Debug && main.exe

.PHONY: clean run
clean:
	rm -f bin/Linux/main

run: ./bin/Linux/main
	cd bin/Linux && ./main
