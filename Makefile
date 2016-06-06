
all:
	gcc -L/usr/local/lib -I/usr/local/include -w -lsndfile -o ocean_noise_detector main.c fft.c

clean:
	-rm -f ocean_noise_detector
	-rm *.out