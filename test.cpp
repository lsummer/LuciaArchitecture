#include <iostream>
using namespace std;
int main(int argc, char* argv[])
{
	#ifdef __linux__
		cout<<"linxu环境" <<endl;
	#elif __APPLE__
		cout<< "macos环境" << endl;
	#else
		cout<<"其他环境" <<endl;
	#endif
	return 0;
}