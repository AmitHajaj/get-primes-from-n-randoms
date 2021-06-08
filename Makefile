deafult:
	gcc -c solution.c
	gcc solution.c -o solution -pthread -lm

	gcc -c rm.c
	gcc rm.c -o rm -lm

	# time ./solution 6 10000
	# time ./solution 6 10000000
	# time ./solution 7 10000000
	# time ./solution 9 10000000
	# time ./solution 12 100000000

	# #Bonus part
	# time ./solution 55 55555555

clean:
	rm *.o sol sol
