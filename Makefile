all:
	g++ -o snakegame main.cpp map.cpp -lncurses

clean:
	rm -f snakegame