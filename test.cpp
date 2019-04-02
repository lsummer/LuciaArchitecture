#include <iostream>
#include <thread>
using namespace std;

// mutex mutex1;
// condition_variable cond;
// unique_lock<mutex> ulock(mutex1);
// int label = 0;
// void funcw(int& i){
// 	while(i<100){
// 		cond.wait(ulock, [](){return label == 0;});
// 		cout<<this_thread::get_id()<<": " << i <<endl;
// 		i++;
// 		label = 1;
// 		cond.notify_one();
// 	}
// }
// void funcr(int& i){
// 	while(i<100){
// 		cond.wait(ulock, [](){return label == 1;});
// 		cout<<this_thread::get_id() << ":" << i<<endl;
// 		label = 2;
// 		i++;
// 		cond.notify_one();
		
		
		
// 	}
// }
// void func3(int& i){
// 	while(i<100){
// 		cond.wait(ulock, [](){return label == 2;});
// 		cout<<this_thread::get_id() << ":" << i<<endl;
		
// 		i++;
// 		label = 0;
// 		cond.notify_one();
// 	}
// }	

// vector<int>  binaryIndexedTree(const vector<int>& vec){
// 	vector<int> binarytree(vec.size()+1, 0);

// 	for(int i=0; i<vec.size(); i++){
// 		binarytree[i+1] = vec[i];
// 	}

// 	for(int i=1; i<vec.size()+1; i++){
// 		int j = i + (i & (-i));
// 		if(j < vec.size()+1){
// 			binarytree[j] += binarytree[i];
// 		}
// 	}
// 	return binarytree;
// }

// void updateBIT(int index, int value, vector<int>& BIT){
// 	index+=1;
// 	while(index < BIT.size()){
// 		BIT[index] += value;
// 		index = index + (index & (-index));
// 	}
// }

// int prefixSum(int index, vector<int>& BIT){
// 	index += 1;
// 	int sum = 0;
// 	while(index > 0){
// 		sum += BIT[index];
// 		index  = index - (index & (-index));
// 	}
// 	return sum;
// }

// int rangesum(int begin, int end, vector<int>& BIT){
// 	return prefixSum(end) - prefixSum(begin-1);
// }

void func(){
	cout << "线程测试" << this_thread::get_id() << endl; 
}
struct th{
	thread* t;
	th(thread* t):t(t){
		
	}
	~th(){}
};
int main(int argc, char* argv[])
{
	// thread* t1 = NULL;
	// th* ts = new th(t1);
	// cout << ts->t << endl; 
	// t1 = new thread(func);
	// t1->join();
	// ts->t = t1;
	// cout << ts->t << endl; 
	// #ifdef __linux__
	// 	cout<<"linxu环境" <<endl;
	// #elif __APPLE__
	// 	cout<< "macos环境" << endl;
	// #else
	// 	cout<<"其他环境" <<endl;
	// #endif

	// int i = 0;
	
	// thread t1(funcw, ref(i));
	// thread t2(funcr, ref(i));
	// thread t3(func3, ref(i));
	// t1.join();
	// t2.join();
	// t3.join();
	char x[10];
	char tmp[] = "I love China";
	x[0] = 'I';
	x[1] = 'l';
	x[2] = '\0';
	// x = tmp;
	printf("%zu", strlen(x));
	memset(x, 0, 10);
	printf("%zu", strlen(x));
	cout <<x <<endl;
	return 0;
}