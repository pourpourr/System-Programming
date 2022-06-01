CC = g++ -std=c++11
OBJ = vaccineMonitor  main.o bloomfilter.o hashing.o skipList.o records.o virusData.o countryStats.o
all:  vaccineMonitor



vaccineMonitor: main.o bloomfilter.o hashing.o skipList.o records.o virusData.o countryStats.o
	$(CC) -o vaccineMonitor main.o bloomfilter.o hashing.o skipList.o records.o virusData.o countryStats.o





main.o:main.cpp
		$(CC)  main.cpp -c

bloomfilter.o:bloomfilter.cpp
		$(CC)  bloomfilter.cpp -c

hashing.o:hashing.cpp
		$(CC)  hashing.cpp -c

countryStats.o:countryStats.cpp
		$(CC)  countryStats.cpp -c

virusData.o:virusData.cpp
		$(CC)  virusData.cpp -c

records.o:records.cpp
		$(CC)  records.cpp -c

skipList.o:skipList.cpp
		$(CC)  skipList.cpp -c




clean:
	rm -f $(OBJ)
