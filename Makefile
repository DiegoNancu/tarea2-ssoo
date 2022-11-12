#makefile tarea 2 so

all:
	g++ -o run main.cpp -lpthread
	./run archivo_listado_ips.txt 4
build:
	g++ -o run main.cpp -lpthread
run:
	./run archivo_listado_ips.txt 4
clean:
	rm run
