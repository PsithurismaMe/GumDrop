all:
	g++ main.cpp -lraylib -O3 -o Platformer
	g++ levelDesigner.cpp -lraylib -O3 -o Level\ Editor
clean:
	rm -f Platformer
	rm -f Level\ Editor