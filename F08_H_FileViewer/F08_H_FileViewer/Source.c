/* (C) IT Sky Consulting GmbH 2014
* http://www.it-sky-consulting.com/
* Author: Karl Brodowsky
* Date: 2014-02-27
* License: GPL v2 (See https://de.wikipedia.org/wiki/GNU_General_Public_License )
*
* access console even when
*
* Source for File Open part from https://msdn.microsoft.com/en-us/library/windows/desktop/bb540534%28v=vs.85%29.aspx
*
*/

#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <stdio.h>
#include <strsafe.h>

#define BUFFERSIZE 256
DWORD g_BytesTransferred = 0;


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

int main(int argc, char *argv[]) {
	BOOL result;
	//const char *msg = "Hello, World!\n(CreateFile/WriteFile/CloseHandle)\n";
	//const char *fileName = "../temp/test-w-not2.txt";

	printf("\n");
	if (argc != 3)
	{
		printf("Usage: shareoption filename\n \
					shareoption e: exclusive \n \
					shareoption s: shareable\n \
					shareoption d: delete\n \
					default shareoption is read");
		return;
	}


	HANDLE fh1;

	DWORD errorCode;
	//DWORD nWritten;

	DWORD  dwBytesRead = 0;
	char   ReadBuffer[BUFFERSIZE] = { 0 };
	char shareMode = FILE_SHARE_READ;
	OVERLAPPED ol = { 0 };

	char inputShareMode = argv[1][0];

	if (inputShareMode == 'e'){
		shareMode = 0;
	}
	else if (inputShareMode == 's'){
		shareMode = FILE_SHARE_WRITE;
	}
	else if (inputShareMode == 'd'){
		shareMode = FILE_SHARE_DELETE;
	}
	else{
		printf("Unknown Share Mode: %s", argv[1]);
	}

	fh1 = CreateFile(argv[2],          /* lpName */
		GENERIC_READ, /* or GENERIC_READ: dwAccess */
		shareMode, /* share mode: not shared: dwShareMode */
		NULL, /* lp security attributes (here alway null) */
		OPEN_EXISTING, /* or CREATE_NEW OPEN_EXISTING, OPEN_ALWAYS, TRUNCATE_EXISTING dwCreate */
		FILE_ATTRIBUTE_NORMAL, /* or FILE_ATTRIBUTE_READONLY or FILE_FLAGE_DELETE_ON_CLOSE or FILE_FLAG_OVERLAPPED (assync I/O) .... dwAttrAndFlags */
		NULL); /* hTemplateFile */

	if (fh1 == INVALID_HANDLE_VALUE){
		printf("Error: CreateFile(..) ");
		//HandleError("CreateFile(..)", FALSE );
	}

	BOOL has_more_data = TRUE;
	while (has_more_data){
		result = ReadFile(fh1, ReadBuffer, BUFFERSIZE - 1, &dwBytesRead, NULL);
		if (!result){
			//printf("Error: ReadFile Error");
			HandleError("ReadFile Error", !result);
			CloseHandle(fh1);
			return;
		}

			
		if (dwBytesRead > 0 && dwBytesRead <= BUFFERSIZE - 1){
			ReadBuffer[dwBytesRead] = '\0';

			printf("Data read from %s (%d bytes): \n", argv[2], dwBytesRead);
			printf("%s\n", ReadBuffer);
		}
		else if (dwBytesRead == 0) {
			printf("No data read from file %s\n", argv[2]);

		}
		else {
			printf("Unexpected value for dwBytesRead \n");
		}

		if (dwBytesRead <= 0 )
			has_more_data = FALSE;
	}
	SleepEx(10000, TRUE);
	dwBytesRead = g_BytesTransferred;




	CloseHandle(fh1);
	//HandleError("fh1=CreateFile(...) -> errorCode=%ld\n", (fh1 == INVALID_HANDLE_VALUE));

	//nWritten = 0;
	//result = WriteFile(fh1, /* HANDLE */
	//	(void *)msg, /* lpBuffer */
	//	(DWORD)strlen(msg), /* nNumberOfBytesToRead */
	//	&nWritten, /* lpNumberOfBytesWritten */
	//	NULL); /* lpOverlapped */
	//HandleError("WriteFile(...)", !result);
	//result = CloseHandle(fh1);
	//HandleError("CloseHandle(fh1)", !result);

	//printf("n=%d\n", nWritten);

	ExitProcess(0);
}

