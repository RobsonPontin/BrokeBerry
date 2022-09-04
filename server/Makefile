CC = gcc
FLAGS = -c

SOURCEDIR = /brokeberry
BUILDDIR = bin

# Any time the objects on output change. Recompile the following files using gcc
# All items in the recipe (output:) will be called to be compiled
output: brokeberry.o host_interface.o ui_interface.o broker.o dio.o dio_functions.o csv_generation.o daemon.o lib_brokeberry.a
	$(CC) brokeberry.o lib_brokeberry.a -lwiringPi -lmosquitto -o brokeberry

brokeberry.o: brokeberry.c
	$(CC) $(FLAGS) brokeberry.c

broker.o: broker.c broker.h
	$(CC) $(FLAGS) broker.c

host_interface.o: host_interface.c host_interface.h
	$(CC) $(FLAGS) host_interface.c

ui_interface.o: ui_interface.c ui_interface.h
	$(CC) $(FLAGS) ui_interface.c

dio.o: dio.c dio.h
	$(CC) $(FLAGS) dio.c
	
dio_functions.o: dio_functions.c dio_functions.h
	$(CC) $(FLAGS) dio_functions.c
	
csv_generation.o: csv_generation.c csv_generation.h
	$(CC) $(FLAGS) csv_generation.c
	
daemon.o: daemon.c daemon.h
	$(CC) $(FLAGS) daemon.c

#archive - Create a static library file as: lib_brokeberry.a
lib_brokeberry.a: broker.o host_interface.o ui_interface.o dio.o dio_functions.o csv_generation.o daemon.o
	ar -cvq lib_brokeberry.a broker.o host_interface.o ui_interface.o dio.o dio_functions.o csv_generation.o daemon.o

clean:
	rm *.o brokeberry lib_brokeberry.a
