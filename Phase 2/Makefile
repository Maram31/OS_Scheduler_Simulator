build:
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out -lm
	#gcc scheduler-beta.c -o scheduler-beta.out -lm
	gcc process.c -o process.out
	#gcc process_ha.c -o process_ha.out
	gcc process_mh.c -o process_mh.out
	gcc test_generator.c -o test_generator.out
	#gcc process_generator_ha.c -o process_generator_ha.out
	#gcc RR_scheduler.c -o RR_scheduler.out -lm
	#gcc srtn_schedular.c -o srtn_schedular.out 
	gcc buddy.c -o buddy.out -lm

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out
	#./buddy.out
