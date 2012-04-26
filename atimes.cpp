#include <cmath>
#include <cstdio>
#include <algorithm>
#include <boost/random.hpp>
#include <ctime>
#include <signal.h>
#include <err.h>
#include "cycle.h"

using namespace std;

const int MEASUREMENTS = 2048;
const int n0 = 1024;
const double inc = 1.05;
double measurements[MEASUREMENTS];
int measured;

void printstats(int);

int main() {
	int num = n0;
	boost::mt19937 rng(time(NULL));
	printf("n\tt (s)\tt/n (ns)\tt/(n ln n)\n");
	fill(measurements+0, measurements+MEASUREMENTS, 0.0);
	measured = 0;
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = printstats;
	if (sigaction(SIGINT, &sa, NULL)) err(1, "sigaction");
	if (sigaction(SIGHUP, &sa, NULL)) err(1, "sigaction");
	if (sigaction(SIGQUIT, &sa, NULL)) err(1, "sigaction");
	for (int i = 0; i < MEASUREMENTS; ++i) {
		void ** addresses = new (nothrow) void*[num];
		if (addresses == 0) break;
		addresses[0] = addresses;
		for (int i = 1; i < num; ++i) {
			// invariant: 0..i-1 are a cycle of pointers
			// insert into cycle at [j]
			int j = rng() % i;
			addresses[i] = addresses[j];
			addresses[j] = addresses+i;
		}
		ticks t0, t1;
		t0 = getticks();
		void ** p = static_cast<void**>(addresses[0]);
		while (p != addresses) {
			p = static_cast<void**>(*p);
		}
		t1 = getticks();
		double e = elapsed(t1, t0);
		printf("%d\t%.9f\t%.2f\t%.2f\n", num, e*1e-9, e/num, e/(num*log(num)));
		fflush(stdout);
		measurements[measured++] = e/num;
		num = num * inc;
		delete[] addresses;
	}
	printstats(0);
	return 0;
}

void printstats(int) {
	const int downsample = 2;
	const int columns = measured/downsample;
	double * downsampled = new double[columns];
	for (int i = 0; i < columns*downsample; ++i) {
		double m = measurements[i];
		downsampled[i] += m/downsample;
	}
	double min = downsampled[0];
	double max = downsampled[0];
	for (int i = 0; i < columns; ++i) {
		double m = downsampled[i];
		if (m > max)
			max = m;
		if (m < min)
			min = m;
	}
	const int rows = 40;
	const int subdivisions = 3;
	const double step = pow(min/max, 1.0/(rows*subdivisions));
	double upper = max;
	printf("\nGraph: Average time per element (log/log)\n");
	for (int i = 0; i < rows; ++i) {
		double t1 = upper*step;
		double t2 = t1*step;
		double t3 = t2*step;
		for (int j = 0; j < columns; ++j) {
			double m = downsampled[j];
			if (t1 < m && m < upper) {
				putc('\'', stdout);
			} else if (t2 < m && m < t1) {
				putc('-', stdout);
			} else if (t3 < m && m < t2) {
				putc(',', stdout);
			} else {
				putc(' ', stdout);
			}
		}
		putc('\n', stdout);
		upper = t3;
	}
	fflush(stdout);
	exit(0);
}
