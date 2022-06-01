CC = g++ -std=c++11
OBJ = travelMonitor  main.o hashing.o  records.o monitor fileHandler.o bloomfilter.o monitor.o  virusData.o 
all:  travelMonitor  monitor



travelMonitor: main.o  fileHandler.o virusData.o records.o bloomfilter.o
	$(CC) -o travelMonitor main.o  fileHandler.o virusData.o records.o bloomfilter.o

monitor: monitor.o  fileHandler.o virusData.o records.o bloomfilter.o
	$(CC) -o monitor monitor.o fileHandler.o virusData.o records.o bloomfilter.o




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
	rm -f $(OBJ)
