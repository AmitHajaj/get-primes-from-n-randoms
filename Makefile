all:
	gcc -c solution.c
	gcc solution.c -o sol -pthread -lm

	gcc -c primeSlow.c
	gcc primeSlow.c -o solution -pthread -lm

clean:
	rm *.o sol2
