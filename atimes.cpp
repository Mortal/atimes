#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include "cycle.h"

using namespace std;

int main(int argc, char ** argv) {
	int num;
	stringstream(argv[1]) >> num;
	while (true) {
		int ** addresses = new int*[num];
		int * nums = new int[num];
		for (int i = 0; i < num; ++i) {
			addresses[i] = nums+i;
			nums[i] = i;
		}
		std::random_shuffle(addresses+0, addresses+num);
		ticks t0, t1;
		t0 = getticks();
		for (int j = 0; j < 10; ++j) {
			for (int i = 0; i < num; ++i) {
				*addresses[i] = i;
			}
		}
		t1 = getticks();
		cout << num << ' ' << fixed << setprecision(0) << elapsed(t1, t0) << endl;
		num = num * 1.3;
		delete[] nums;
		delete[] addresses;
	}
	return 0;
}
