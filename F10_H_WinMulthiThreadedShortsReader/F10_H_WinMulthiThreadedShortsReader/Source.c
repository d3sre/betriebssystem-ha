/* (C) IT Sky Consulting GmbH 2014
* http://www.it-sky-consulting.com/
* Author: Karl Brodowsky
* Date: 2014-02-27
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
*
* create a process
*/

#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>

#include <signal.h>
#include <WinBase.h>
//#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/wait.h>
#include <time.h>
//#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define BETA_SIZE 65536
#define BUF_SIZE 16384
#define SEM_SIZE 128

LONG count = 0;

struct data2 {
	LONG counter[BETA_SIZE];
};
const int SIZE2 = sizeof(struct data2);

static struct data2 data2;
HANDLE mutexhandle;
const char *output_mutex;
const char *the_mutex;



struct tdata {
	DWORD idx;
	LONG  tnum;
	DWORD duration;
	DWORD tid;
	HANDLE handle;
};

VOID HandleError(LPCTSTR msg, BOOL failure) {
	if (failure) {
		DWORD errorCode = GetLastError();
		LPTSTR errMsg;
		DWORD msgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&errMsg,
			0,
			NULL);
		if (msgLen > 0) {
			printf("%s: errorCode=%ld %s\n", msg, (long)errorCode, errMsg);
		}
		else {
			printf("%s: errorCode=%ld\n", msg, (long)errorCode);
		}
		if (errMsg != NULL) {
			LocalFree(errMsg);
		}
		ExitProcess(1);
	}
}


DWORD WINAPI MyThreadFunction(LPVOID param);



int main(int argc, char *argv[]) {
	HANDLE* pThreadId = malloc((argc - 1)*sizeof(HANDLE));

	//DWORD threadId[argc];
	if (argc < 2) {
		printf("Usage\n\n");
		printf("%s file1 file2 file3 ... filen\ncount files, show accumulated output after having completed one file\n\n", argv[0]);
		exit(1);
	}
	BOOL result;

	DWORD errorCode;
	DWORD nWritten;

	struct tdata threadData[argc];

	DWORD MAX_TIME = 100000;

	time_t start_time = time(NULL);
	HANDLE retcode;
	printf("%d files will be read\n", argc - 1);
	fflush(stdout);


	int i;

	for (i = 0; i < BETA_SIZE, i++) {
		data2.counter[i] = 0L;
	}
	//retcode = OpenMutex(
	//	SYNCHRONIZE,
	//	TRUE,
	//	output_mutex
	//	);
	mutexhandle = CreateMutex(
		NULL,
		TRUE,
		the_mutex
		);
	if (mutexhandle == NULL){
		printf("CreateMutex error: %d\n", GetLastError());
	}

	

	for (i = 0; i < argc; i++) {
		DWORD duration = rand() % MAX_TIME;
		threadData[i].duration = duration;
		threadData[i].idx = i;
		printf("creating thread i=%d d=%d\n", i, duration);
		threadData[i].handle = CreateThread(NULL, /* LPSECURITY_ATTRIBUTES lpThreadAttributes, */
			0, /* SIZE_T dwStackSize, */
			MyThreadFunction, /* LPTHREAD_START_ROUTINE lpStartAddress, */
			&threadData[i], /* LPVOID lpParameter, */
			0, /* DWORD dwCreationFlags, */
			&threadData[i].tid); /* LPDWORD lpThreadId */
		printf("thread created i=%d threadId=%d i=%d\n", i, threadData[i].tid);
	}

	DWORD dwWaitResult = WaitForSingleObject(mutexhandle, INFINITE);
	printf("%d threads started\n", argc - 1);
	fflush(stdout);

	ReleaseMutex(mutexhandle);

	Sleep(1000);
	HANDLE handles[argc];
	for (i = 0; i < argc; i++) {
		handles[i] = threadData[i].handle;
	}
	result = WaitForMultipleObjects(argc, handles, TRUE, MAX_TIME);
	printf("threads waited result=%ud ", result);
	switch (result) {
	case WAIT_ABANDONED:
		printf("WAIT_ABANDONED");
		break;
	case WAIT_OBJECT_0:
		printf("WAIT_OBJECT_0");
		break;
	case WAIT_TIMEOUT:
		printf("WAIT_TIMEOUT");
		break;
	case WAIT_FAILED:
		printf("WAIT_FAILED");
		break;
	default:
		printf("OTHER");
	}
	printf("\n");
	Sleep(3000);
	ExitProcess(0);
}


DWORD WINAPI MyThreadFunction(LPVOID param) {
	//struct tdata *tdata = (struct tdata *) param;
	//LONG num = InterlockedIncrement(&count);
	//tdata->tnum = num;
	//DWORD duration = tdata->duration;
	//printf("in thread i=%d n=%d t=%d\n", (int)tdata->idx, tdata->tnum, tdata->tid);
	//new
	time_t thread_start = time(NULL);
	char *name = (char *) param;
	HANDLE fd = -1;
	time_t open_start = time(NULL);

	while (fd == -1){
		fd = CreateFile(name,          /* lpName */
			GENERIC_READ, /* or GENERIC_READ: dwAccess */
			FILE_SHARE_READ, /* share mode: not shared: dwShareMode */
			NULL, /* lp security attributes (here alway null) */
			OPEN_EXISTING, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
			FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
			NULL); /* hTemplateFile */

		if (fd < 0 && errno == EMFILE) {
			sleep(1);
			continue;
		}
		if (fd < 0) {
			char msg[256];
			sprintf(msg, "error while opening file=%s", name);
			HandleError(msg, 1);
		}
	}

	//measure time open
	time_t open_duration = time(NULL) - open_start;
	time_t total_mutex_wait = 0;

	SHORT shortValue;

	struct data2 local_data2;
	LONG *scounter2 = local_data2.counter;

	while (TRUE){
		SSIZE_T size_read = ReadFile(fd, &shortValue, sizeof(shortValue));
		if (size_read == 0) {
			//end of file
			break;
		}
		if (size_read < 0){
			sprintf("error occured while reading file\n");
		}
		if (size_read == 1){
			printf("file contains a remaining byte, will be discarded");
		}
		else {
			scounter2[shortValue]++;
		}
	}

	CloseHandle(fd);

	time_t thread_duration = time(NULL) - thread_start;
	unsigned int i;
	LONG *tcounter2 = data.counter;
	mutexhandle = OpenMutex(
		SYNCHRONIZE,
		TRUE,
		output_mutex
		);
	printf("------------------------------------------------------------\n");
	printf("%s: pid=%ld\n", name, (long)getpid());
	printf("open duration: ~ %ld sec; total wait for data: ~ %ld sec; thread duration: ~ %ld\n", (long)open_duration, (long)total_mutex_wait, (long)thread_duration);
	printf("------------------------------------------------------------\n");
	for (i = 0; i < BETA_SIZE; i++) {
		tcounter2[i] += scounter2[i];
		long val = tcounter2[i];
		if (val != 0) {
			if (i <= 32 || i >= 127) {
				printf("\\%06o: %20ld \n", i, val);
			}
			else {
				//can not be reached as 2 ascii chars would need to be in a row, where NUL would be the lead character and NUL can't be typed into a file
				printf("%7c: %20ld \n", (char)i, val);
			}
		}
	}
	printf("\n\n");
	printf("------------------------------------------------------------\n\n");
	fflush(stdout);

	ReleaseMutex(mutexhandle);
	//Sleep(duration);
	//printf("done with thread i=%d n=%d t=%d\n", (int)tdata->idx, tdata->tnum, tdata->tid);
	return NULL;
}