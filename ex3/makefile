CC = g++ -std=c++11 -pthread -g
OBJ = travelMonitorClient  main.o hashing.o  records.o monitorServer fileHandler.o bloomfilter.o monitor.o  virusData.o 
all:  monitorServer  travelMonitorClient



travelMonitorClient: main.o  fileHandler.o virusData.o records.o bloomfilter.o
	$(CC) -o travelMonitorClient main.o  fileHandler.o virusData.o records.o bloomfilter.o

monitorServer: monitor.o  fileHandler.o virusData.o records.o bloomfilter.o
	$(CC) -o monitorServer monitor.o fileHandler.o virusData.o records.o bloomfilter.o




main.o:main.cpp
		$(CC)  main.cpp -c

monitor.o:monitor.cpp
		$(CC)  monitor.cpp -c

hashing.o:hashing.cpp
		$(CC)  hashing.cpp -c


fileHandler.o:fileHandler.cpp
		$(CC)  fileHandler.cpp -c

records.o:records.cpp
		$(CC)  records.cpp -c

virusData.o:virusData.cpp
		$(CC)  virusData.cpp -c

bloomfilter.o:bloomfilter.cpp
		$(CC)  bloomfilter.cpp -c


clean:
	rm -f $(OBJ) log*
#	rm -f log*
