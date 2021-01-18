build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out -lm
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out
	gcc buddy.c -o buddy.out -lm

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out
	#./buddy.out
