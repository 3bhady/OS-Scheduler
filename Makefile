build: 
	g++ clk.cpp -o clock.out
	g++ scheduler.cpp -o sch.out
	g++ FilesGenerator.cpp -o Files.out
	g++ processGenerator.cpp -o main.out
	g++ OS.cpp  -o os.out

clean:
	rm -f *.out  processes.txt
	ipcrm -M 300
	ipcrm -Q 777
	@echo "All Items Cleared"

all: clean build

run:
	./main.out
