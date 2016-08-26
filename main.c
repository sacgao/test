#include "stdio.h"
#include "malloc.h"
#include "string.h"
#include "winsock2.h"
#include "windef.h"

#pragma  comment(lib,"WS2_32.lib")

#define  FALSE 0
#define  TRUE  1

unsigned int IEC104ListenProc();

typedef  unsigned char  bool;

struct test_size1 {
	char a;
	char b;
	char d;
	char c;
	int  e;
};

struct test_size2 {
	char a;
	int  e;
	char b;
	char d;
	char c;
};

struct test_size3{
	char a;
	int  e;
	char b;
};

int getDir(char ***filenames, int *filenum)
{
	char ca_filename[255];
	bool more_follows = FALSE; 
	int i;
	char**	filenames2;
	int  num_dir_ent =120;
	char *dir_ent[]= {
		"1DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"2COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"3[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"4[--version] print program version",
		"5[--help] print help information",
		"6[--file name] external config by file(Protocol_DL476.ini), version...",
		"7DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"8COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"9[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"10[--version] print program version",
		"11[--help] print help information",
		"12[--file name] external config by file(Protocol_DL476.ini), version...",
		"13DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"14COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"15[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"16[--version] print program version",
		"17[--help] print help information",
		"18[--file name] external config by file(Protocol_DL476.ini), version...",
		"19[--help] print help information",
		"20[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"119[--help] print help information",
		"120[--file name] external config by file(Protocol_DL476.ini), version..."
	};
	char *dir_ent1[] = {
		"121[--file name] external config by file(Protocol_DL476.ini), version...",
		"122[--help] print help information",
		"123[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"[--version] print program version",
		"[--help] print help information",
		"[--file name] external config by file(Protocol_DL476.ini), version...",
		"DESCRIPTION:Delivery information of remote browse through protocol DL476",
		"COMMAND:Protcocol_DL476_RBrowse [--channel...] [--version] [--help] [--file]",
		"152[--channel obid1,obid2...] DL476ServerChannel ObId included",
		"153[--version] print program version"
	};
	
	*filenum = 0; 

	filenames2 = calloc(*filenum+num_dir_ent, sizeof(char*));
	for ( i = 0; i < *filenum; ++i)
		filenames2[i] =(*filenames)[i];
	(*filenames) = filenames2;

	for (i = 0; i < num_dir_ent;  i++)
	{
		//printf ("\n   File #%d: %s", i, fdir_resp->dir_ent[i].filename);
		(*filenames)[*filenum+i] = strdup(dir_ent[i]);
	}  
	if (num_dir_ent >0 ) 
		strcpy(ca_filename,dir_ent[num_dir_ent-1]);
	*filenum += num_dir_ent;

	num_dir_ent = 33;
	filenames2 = calloc(*filenum+num_dir_ent, sizeof(char*));
	for ( i = 0; i < *filenum; ++i)
		filenames2[i] =(*filenames)[i];
	(*filenames) = filenames2;

	for (i = 0; i < num_dir_ent;  i++)
	{
		//printf ("\n   File #%d: %s", i, fdir_resp->dir_ent[i].filename);
		(*filenames)[*filenum+i] = strdup(dir_ent1[i]);
	}  
	if (num_dir_ent >0 ) 
		strcpy(ca_filename,dir_ent1[num_dir_ent-1]);
	*filenum += num_dir_ent;


	return 0;
}

//main()
//{
//	//printf("hello yuan zong\n");
//	//getchar();
//	//char		**filenames;
//	//int			filenum ;
//	//getDir(&filenames, &filenum);
//
//	char *s="20121011_140001_780";
//    char *p;
//	unsigned char ms[4];
//	int	i;
//	unsigned short a = 0x7ff0;
//	float          b = 1.5f;
//	short          c1;
//	unsigned short c11;
//	int			   c2;
//
//	c1 = a*b;
//	c11 = a*b;
//    c2 = a*b;
//
//	printf("%d %d %d\n", c1, c11, c2);
//
//    p=strrchr(s,'_');
//	if(p) {
//      printf("%s\n",p);
//	  memset(ms, 0, sizeof(ms));
//	  strncpy(ms, p, 4);
//	  while(ms[0]=='0' || ms[0]=='_') {
//		  memmove(ms, ms+1, strlen(ms));
//	  }
//	  i = atoi(ms);
//	  printf("%d\n",i);
//	/*  memset(ms, 0, sizeof(ms));
//	  strncpy(ms, p+1, 3);
//	  i = atoi(ms);
//	  printf("%d\n",i);*/
//	}
//    else
//      printf("Not Found!");
//	
//    getchar();
//    return 0;
//}


/* check is leap year */
#define IS_LEAP_YEAR(year) (((year)%400 == 0) || (((year)%4 == 0) && ((year)%100 != 0)))

/* return month(1~12, others is invalid) */
unsigned char getMonth(int year, int yday)
{
	int monthDay[13]={-1,30,58,89,119,150,180,211,242,272,303,333,364};
	int leapMonthDay[13]={-1,30,59,90,120,151,181,212,243,273,304,334,365};
	int md[13]= {0};
	int month = 0;
	int *tmp_p;

	if (IS_LEAP_YEAR(year)) 
	{
		for (month=0; month<13; month++) {
			md[month] = leapMonthDay[month];
		}
	}
	else 
	{
		for (month=0; month<13; month++) {
			md[month] = monthDay[month];
		}
	}
	tmp_p = md;
	for(month=0;month<12;month++)
	{
		if ((yday>tmp_p[month]) && (yday<=tmp_p[month+1]))
		{
			return month+1;	
		}
	}
	printf("%d year's %d day erro\n", year, yday);
	return month;
}

int main(int argc, const char **argv)
{
	int year,yday;

	printf("%d, %d, %d\n", sizeof(struct test_size1), sizeof(struct test_size2), sizeof(struct test_size3));
	
	IEC104ListenProc();

	printf("input year, day of year(0~355),\n");
	scanf("%d %d",&year,&yday);

	printf("%d year's %d day is %d month\n", year, yday, getMonth(year, yday));

	return 0;
}

void error_abort()
{
	printf("error \n");
}
unsigned int IEC104ListenProc( )
{ 
	SOCKET listensock; ///< 监听的Socket
	int maxbufsize = 1000;
	int reuseaddr=1,keepvalue=1;
	struct sockaddr_in localaddr; ///< 服务器本机地址
	struct sockaddr_in clientaddr; ///< 客户机地址
	struct timeval     selecttv;
	fd_set  readfds;
	int threadno=-1;//当前线程在创建的线程组中的序号
	int len;
	WSADATA wsaData;    // 这结构是用于接收Wjndows Socket的结构信息的   
	int err;  

	ULONG uIP = inet_addr("172.21.140.39");	
	memset((char *)(&localaddr), 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	//localaddr.sin_port = htons(2404);
	localaddr.sin_port = 2404;
	//localaddr.sin_addr.s_addr = htonl(uIP);  
	localaddr.sin_addr.s_addr = uIP;  

	err = WSAStartup( 0x0202, &wsaData );  
	if ( err != 0 ) {  
		return -1;          // 返回值为零的时候是表示成功申请WSAStartup   
	}  

	//if ( LOBYTE( wsaData.wVersion ) != 1 || HIBYTE( wsaData.wVersion ) != 1 ) {  
	//	// 检查这个低字节是不是1，高字节是不是1以确定是否我们所请求的1.1版本   
	//	// 否则的话，调用WSACleanup()清除信息，结束函数   
	//	WSACleanup( );  
	//	return -1;   
	//} 
	//构建侦听Socket
	listensock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(listensock == SOCKET_ERROR) {
		printf("转发Socket服务端构建失败\n");
		return 0;
	}

	//设置socket option
	if(setsockopt(listensock,SOL_SOCKET,SO_RCVBUF,(char *)&maxbufsize,sizeof(maxbufsize))<0)  
		error_abort(); //SO_RCVBUF means to Specify buffer size for receives 
	if(setsockopt(listensock,SOL_SOCKET,SO_SNDBUF,(char *)&maxbufsize,sizeof(maxbufsize))<0)  
		error_abort(); //SO_SNDBUF means to Specify buffer size for sends 
	if(setsockopt(listensock,SOL_SOCKET,SO_REUSEADDR,(char *)&reuseaddr,sizeof(reuseaddr))<0)  
		error_abort(); //SO_REUSEADDR means to Allow the socket to be bound to an address that is already in use
	if(setsockopt( listensock, SOL_SOCKET, SO_KEEPALIVE, (char *)&keepvalue, sizeof(keepvalue)) < 0 )   
		error_abort(); //SO_KEEPALIVE means to Send keepalives (连接活跃)

	//if(bind(listensock,(struct sockaddr *)&localaddr,sizeof(localaddr))<0)   
	err = bind(listensock,(struct sockaddr *)&localaddr,sizeof(struct sockaddr_in));
	err = GetLastError();
	if(err<0)   
		error_abort(); //bind fail,exit.

	if(listen(listensock,SOMAXCONN) != 0) 
		error_abort(); //places a socket a state where it is listening for an incoming connection.


	return TRUE;
}
