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
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
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
// std::string GetGmtTime()
// {
// 	char szGmtTime[50];
	
// 	time_t rawTime;
// 	struct tm* timeInfo;
// 	char szTemp[30]={0};
// 	time(&rawTime);
// 	timeInfo = gmtime(&rawTime);
// 	strftime(szTemp,sizeof(szTemp),"%a, %d %b %Y %H:%M:%S GMT",timeInfo);
// 	strcpy(szGmtTime, szTemp);//必须这样，避免内存释放，内容没有的问题。 
// 	// strncpy_s(szGmtTime, sizeof(szGmtTime), szTemp, strlen(szGmtTime)+1);
// 	string time_str(szGmtTime, strlen(szGmtTime));
// 	return time_str;
// }
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


	// string str("\\.(js|css|png|ico|jpg|jpeg|gif|html|json|eot|svg|ttf|woff|txt|gz|mp3|mp4|mk)$");
	// std::regex txt_regex(str, std::regex::icase);
	// std::string fname = "./config.mk";
	// if(std::regex_search(fname, txt_regex)){
	// 	struct stat buf;
	// 	int st = stat(fname.c_str(), &buf);
	// 	int fd = open(fname.c_str(), O_RDONLY);
	// 	char x[1024];
	// 	cout << read(fd, x, 1024) <<endl;
		
	// 	printf("%s", x);
	// 	// close(fd);
	// 	// cout << GetGmtTime(&(buf.st_mtime)) << endl;
	// }

	// std::string value = "header.getHeader()";
    // char* buf = new char[value.length()+2];

    // strcpy(buf, value.c_str()); 
	// printf("%s\n%lu", buf, strlen(buf));


	// FILE* png = fopen("/Users/lxy/Desktop/Arch.png", "r");
	// FILE* wpng = fopen("/Users/lxy/Desktop/Arch_copy.png", "w");

	// struct stat buf;

    
    // if(stat("/Users/lxy/Desktop/Arch.png", &buf) == -1 ){
	// 	return 0;
	// }
	// unsigned char buffer[buf.st_size];
	
	// size_t size = fread(&buffer, sizeof(unsigned char), buf.st_size, png);
	// fwrite(&buffer, sizeof(unsigned char), buf.st_size, wpng);

	// string a="1234688765434123223322";
	// cout <<a <<endl;
	// auto itre = a.find_first_of("123");
	// a.erase(itre, itre+3);
	// cout <<a <<endl;

	// char m[] = "asdfgfd";
	// string x(m, 3);
	// cout << x <<endl;


	// map<string, string> m;
	// string key, value;
	// key = "123";
	// value  = "123";
	// m[key] = value;
	// for(auto x:m){
	// 	cout << x.first << " : " << x.second << endl;
	// }
	string header = "Content-Disposition: form-data; name=\"arch\"";
	// string s = "/v1/test/callservice";
	string name, filename;
	int name_begin = header.find("name=\"") + 6;  
    int name_end = header.find("\"", name_begin);
    if(name_begin != std::string::npos){
        if(name_end != std::string::npos){
            name = header.substr(name_begin, name_end-name_begin);
        }else{
            name = header.substr(name_begin);
        }
    }else{
        name = "";
        filename = "";
        // return;
    }
	cout << name_begin <<endl;
	cout << name << endl;

    int file_name_begin = header.find("filename=\"", name_begin);
    int file_name_end = header.find('"', file_name_begin);
    if(file_name_begin != std::string::npos){
        if(file_name_end != std::string::npos){
            filename = header.substr(file_name_begin, file_name_end - file_name_begin);
        }else{
            filename = header.substr(file_name_begin);
        }
    }else
    {
        filename = "";   
    }
    // LOG_ACC(INFO, "filename = %s", filename.c_str());

	cout << filename<< endl;
	return 0;

}