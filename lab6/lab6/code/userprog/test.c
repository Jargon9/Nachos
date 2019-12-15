#include "syscall.h"

int fd1,fd2;
int result;
char buffer[20];
void ThreadA(){
	char a[2], ch = 'a';
	int i;
	a[0] = 'a';
	a[1] = 'm';
	fd2 = Open("write.txt");
	Write(&ch, 1, fd2);
	Yield();

}
void ThreadB(){
	char a[2], ch = 'a';
	int i;
	a[0] = 'a';
	a[1] = 'm';
	fd2 = Open("read.txt");
	Write(&ch, 1, fd2);
	Yield();

}
int main(){
//	Fork(ThreadA);
//	Fork(ThreadB);

//	char a[5];
//	int code = -1;
//	int sp;
//	a[0] = 't';
//	a[1] = 'e';
//	a[2] = 's';
//	a[3] = 't';
//	a[4] = '\0';
//
//	sp = Exec(a);
//	code = Join(sp);
//	Exit(code);
//	Yield();
//	char a[2];
//	a[0] = 'a';
//	a[1] = 'm';
//
//	Create("write.txt");
	fd1 = Open("read.txt");
//	fd2 = Open("write.txt");
//	result = Read(buffer, 20, fd1);
//	Write(a, 2, fd2);
	Close(fd1);
//	Close(fd2);
//	Halt();
}
