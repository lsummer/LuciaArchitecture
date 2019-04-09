#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <list>
#include <time.h>
#include <stdio.h>
#include<cstring>
#include <string.h>
#include <regex>
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
string str;
void func(const char* x, int length){
	str = string(x, x+length);

	// cout << "线程测试" << this_thread::get_id() << endl; 
}
// struct th{
// 	thread* t;
// 	th(thread* t):t(t){
		
// 	}
// 	~th(){}
// };
std::string GetGmtTime()
{
	char szGmtTime[50];
	
	time_t rawTime;
	struct tm* timeInfo;
	char szTemp[30]={0};
	time(&rawTime);
	timeInfo = gmtime(&rawTime);
	strftime(szTemp,sizeof(szTemp),"%a, %d %b %Y %H:%M:%S GMT",timeInfo);
	strcpy(szGmtTime, szTemp);//必须这样，避免内存释放，内容没有的问题。 
	// strncpy_s(szGmtTime, sizeof(szGmtTime), szTemp, strlen(szGmtTime)+1);
	string time_str(szGmtTime, strlen(szGmtTime));
	return time_str;
}
std::string GetGmtTime(const time_t* rawTime){
	struct tm* timeInfo;
	char szTemp[30]={0};
	timeInfo = gmtime(rawTime);
	strftime(szTemp,sizeof(szTemp),"%a, %d %b %Y %H:%M:%S GMT",timeInfo);

	string time_str(szTemp, strlen(szTemp)+1);
	return time_str;
}
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
	// char x[] = "12345";
	// int len = 6;
	// func(x, len);
	// cout << str << endl;
	// list<int*> x;
	// int* m = new int(1);
	// int* q = new int(2);
	// int* p = new int(3);
	// x.push_back(m);
	// x.push_back(q);
	// x.push_back(p);

	// x.pop_front();
	// cout << (*m) <<endl;
	// cout << &x <<endl;
	// cout << GetGmtTime() << endl;
	// time_t rawTime;
	// time(&rawTime);
	// cout << GetGmtTime(&rawTime) << endl;
	string str("\\.(js|css|png|ico|jpg|jpeg|gif|html|json|eot|svg|ttf|woff|txt|gz|mp3|mp4)$");
	std::regex txt_regex(str, std::regex::icase);
	std::string fname = "servei.js.xx";
	if(std::regex_search(fname, txt_regex)){
		cout << "TTrue" << endl;
	}
	return 0;

}