#include <stdio.h>
#include <windows.h>
#include "rs232/rs232.h"

#define RECV_BUF_LEN 1024

int main(void) {
	char pRecvBuf[RECV_BUF_LEN];
	int lReadByteCount, lResult;

	// Config
	int lPort = 2; // COM3
	int lBaudRate = 9600;
	char szMode[] = "8N1";

	lResult = RS232_OpenComport(lPort, lBaudRate, szMode);
	if(lResult) {
		printf("Couldn't open serial port %d", lPort);
		return 1;
	}

	while(1) {
		lReadByteCount = RS232_PollComport(lPort, pRecvBuf, RECV_BUF_LEN);
		if(lReadByteCount) {
			pRecvBuf[lReadByteCount] = 0;
			printf("Recv %d: ", lReadByteCount);
			for(int i = 0; i != lReadByteCount; ++i) {
				printf("%0x ", pRecvBuf[i]);
			}
			printf("| ");
			printf("%s\n", pRecvBuf);
		}
		Sleep(100);
	}

	return 0;
}
