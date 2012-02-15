-------------------------------------------------------------------------------
-- Project: Data Logger with Ethernet UDP output
-- Author(s): DFC Design, s.r.o. (www.dfcdesign.cz)
-- E-Mail:  info@dfcdesign.cz
-- Created: Sept 2011   
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--                                                                           --
-- Copyright (C) 2011 Authors                                                --
--                                                                           --
-- This source file may be used and distributed without                      --
-- restriction provided that this copyright statement is not                 --
-- removed from the file and that any derivative work contains               --
-- the original copyright notice and the associated disclaimer.              --
--                                                                           --
-- This source file is free software; you can redistribute it                --
-- and-or modify it under the terms of the GNU Lesser General                --
-- Public License as published by the Free Software Foundation;              --
-- either version 3.0 of the License, or (at your option) any                --
-- later version.                                                            --
--                                                                           --
-- This source is distributed in the hope that it will be                    --
-- useful, but WITHOUT ANY WARRANTY; without even the implied                --
-- warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR                   --
-- PURPOSE.  See the GNU Lesser General Public License for more              --
-- details.                                                                  --
--                                                                           --
-- You should have received a copy of the GNU Lesser General                 --
-- Public License along with this source; If not, see                        --
-- <http://www.gnu.org/licenses/>                                            --
--                                                                           --
------------------------------------------------------------------------------- 


#include <stdio.h>
#include <winsock2.h>
#include <process.h>


#define PORT 8200


#define DATA		1280
#define MYHEADER	16
#define PAYLOAD		(DATA + MYHEADER)

#define BUFMUL		10240

unsigned int payload[PAYLOAD/4 * BUFMUL];
volatile unsigned int run = 1;

void stop_thread(void *unused)
	{
	printf("Press enter to stop logging:");
	getchar();
	run = 0;
	}

int main (int argc, char *argv[])
	{
	WSADATA wsaData;
	int sockfd;
	struct sockaddr_in sendaddr;
	int numbytes, i;
	int addr_len;
	char broadcast = 1;
	unsigned int lost=0, next=0, received = 0, cur = 0;
	FILE *f;
	int timeo = 2000;
	int bufsize = 10*1024*1024;


	for(i = 0; i < PAYLOAD/4; i++)
		{
		payload[i] = i;
		}

	f=fopen("data.dat", "wb");
	if (f == NULL)
		{
		printf("Error: can not open output file\n");
		return -1;
		}

	// Initialize Winsock
	WSAStartup(MAKEWORD(2,2), &wsaData);

	
	if((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		printf("Error: can not open socket");
		exit(1);
	}
	
	printf("Socket created\n");

	if((setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,
				&broadcast,sizeof broadcast)) == -1)
	{
		printf("Error: setsockopt - SO_SOCKET failed");
		exit(1);
	}

	if((setsockopt(sockfd,SOL_SOCKET, SO_RCVTIMEO,
				(char*)&timeo,sizeof timeo)) == -1)
	{
		printf("Error: setsockopt - SO_SOCKET failed");
		exit(1);
	}

	if((setsockopt(sockfd,SOL_SOCKET, SO_RCVBUF,
				(char*)&bufsize, sizeof bufsize)) == -1)
	{
		printf("Error: setsockopt - SO_SOCKET failed");
		exit(1);
	}

	// Receive from any address
	sendaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Set UDP port
	sendaddr.sin_port = htons(PORT);
	// Protocol family
	sendaddr.sin_family = AF_INET;

	if (bind(sockfd, (struct sockaddr *)(&sendaddr), sizeof(sendaddr)) < 0)
	{
		printf("Error: can not bind socket\n");
		closesocket(sockfd);
		return(-1);
	}
	printf("Receiver socket bound\n");

	CreateThread(NULL, 10*1024, (LPTHREAD_START_ROUTINE)stop_thread, NULL, 0, NULL);

	while(run){	
		if((numbytes = recv(sockfd, (char*)payload + (PAYLOAD * (received % BUFMUL)), PAYLOAD, 0)) == -1)
			{
			if (WSAGetLastError() == WSAETIMEDOUT)
				continue;
			else
				{
   				printf("Error: recvfrom, failed with error %d", WSAGetLastError());
				break;
				}
			}
		else if (numbytes == 0)
			{
			printf("Error: connection closed by peer\n"); // This should never happen
			break;
			}
		else if (numbytes != PAYLOAD)
			{
			printf("Error: incomplete packet received");
			break;
			}

		
		cur = *(int*)((char*)payload + (PAYLOAD * (received % BUFMUL)));

		received++;
	

		if ((received % BUFMUL) == 0)
			fwrite(payload, PAYLOAD * BUFMUL, 1, f);

/*		if(cur != next)
			{
			lost += cur - next;
			if(cur < next)
				printf("Incorrect packet order, expected %u, came %u, received %u\n", next, payload[0], received);
			}
*/
		
		next = cur + 1;

		}

		if ((received % BUFMUL) != 0)
			fwrite(payload, 1, PAYLOAD * (received % BUFMUL), f);

		printf("Received %u, last %u, lost %u\n",received, cur, lost);
		if (f != NULL)
			{
			fclose(f);
			f = NULL;
			}
		system("PAUSE");
	return 0;
}
