#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <assert.h>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <sstream>
#include <thread>

using namespace std;

string getFName(char prefix, int num){
	stringstream s("");
	s << prefix << "_" << num << ".txt";
	return s.str();
}

void getPartSum(int tid, int sumList[], int f1, int e1, int f2, int e2, int maxNum, char prefix){

	int sum = 0, temp;
	ifstream f;
	
	f.open(getFName(prefix, f1).c_str());
	for(int i = 0 ; i < e1 ; i++)
		f >> temp;
	
	if(f1 == f2){
		//cout<<"ID "<<prefix<<tid<<" checking file "<<f1<<" from "<<e1<<" to "<<e2<<endl;
		for(int i = e1 ; i <= e2 ; i++){
			f >> temp;
			sum += temp;
		}
	}
	else{
		//cout<<"ID "<<prefix<<tid<<" checking file "<<f1<<" from "<<e1<<" to "<<maxNum<<endl;
		for(int i = e1 ; i < maxNum ; i++){
			f >> temp;
			sum += temp;
		}
		f.close();
		f.open(getFName(prefix, f2).c_str());
		//cout<<"ID "<<prefix<<tid<<" checking file "<<f2<<" from 0 to "<<e2<<endl;
		for(int i = 0 ; i <= e2 ; i++){
			f >> temp;
			sum += temp;
		}
	}
	
	f.close();
	
	sumList[tid] = sum;

}

void getFullSum(int tid, int sumList[], int f1, int f2, int maxNum, char prefix){

	int sum = 0, temp;
	ifstream f;
	
	//cout << "ID " << prefix << tid << " checking file " << f1 << " to " << f2 << endl;
	
	for(int fc = f1; fc < f2 ; fc++){
		f.open(getFName(prefix, fc).c_str());
		for(int i = 0 ; i < maxNum ; i++){
			f >> temp;
			sum += temp;
		}
		f.close();
	}
	
	sumList[tid] = sum;

}

void spawnSumThreads(char prefix, int tnum, int fnum, int sumList[], int maxNum){
	
	thread threads[tnum];
	
	if(tnum <= fnum){
		
		int fs = 0;
		for(int i = 0 ; i < tnum ; i++){
			int fe = fs + fnum/tnum + (i < fnum%tnum ? 1 : 0);
			threads[i] = thread(getFullSum, i, sumList, fs, fe, maxNum,prefix);
			fs = fe;
		}
	
	}
	else{
		
		int f = 0, e = 0, n = fnum*maxNum;
		for(int i = 0 ; i < tnum ; i++){
			int ee = e + n/tnum + (i < n%tnum ? 0 : -1);
			int fe = f + ee/maxNum;
			ee = ee%maxNum;
			threads[i] = thread(getPartSum, i, sumList, f, e, fe, ee, maxNum,prefix);
			e = ee + 1;
			f = fe + e/maxNum;
			e = e % maxNum;
		}
	
	}
	
	for(int i = 0 ; i < tnum ; i++)
		threads[i].join();
	
	return;
	
}

void createFiles(char prefix, int fnum, int num){
	ofstream f;
	
	for(int i = 0 ; i < fnum ; i++){
		f.open(getFName(prefix,i).c_str());
		for(int j = 0 ; j < num ; j++)
			f << rand()%100 << " ";
		f.close();
	}
}

int arrSum(int arr[], int n){
	int sum = 0;
	for(int i = 0 ; i < n ; i++)
		sum += arr[i];
	return sum;
}

int main(int argc, char **argv){

	assert(argc == 6);
	
	srand(1);
	
	int f1, f2, num, t1, t2;
	
	
	f1 = atoi(argv[1]);
	f2 = atoi(argv[2]);
	num = atoi(argv[3]);
	t1 = atoi(argv[4]);
	t2 = atoi(argv[5]);
	
	
	createFiles('a', f1, num);
	createFiles('b', f2, num);
	
	int fd[2];
	pipe(fd);
	
	int sum;
	
	if(fork() != 0){		//Parent
		int sumList[t1];
		
		spawnSumThreads('a',t1,f1,sumList,num);
		
		wait(NULL);
		
		close(fd[1]);
		read(fd[0], &sum, sizeof(sum));
		close(fd[0]);
		
		sum += arrSum(sumList,t1);
		
		cout<<"Total Sum = "<<sum<<endl;
	}
	else{
	
		int sumList[t2];
		
		spawnSumThreads('b',t2,f2,sumList,num);
		
		sum = arrSum(sumList,t2);
		close(fd[0]);
		write(fd[1], &sum, sizeof(sum));
		close(fd[1]);
		
	}
	return 0;
}
