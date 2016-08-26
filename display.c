#include <stdio.h>

/*数据类型定义*/
typedef unsigned char 	uint_8;
typedef unsigned short 	uint_16;
typedef unsigned long 	uint_32;
typedef signed char		int_8;
typedef short 			int_16;
typedef long 			int_32;
typedef float			float_t;

#define  MAKEWORD(low,high)  ((uint_16)((uint_8)(low)|(((uint_16)((uint_8)(high)))<< 8)))
#define  MAKEDWORD(low,high) ((uint_32)(((uint_16)(low))|(((uint_32)((uint_16)(high)))<< 16)))

//bmp attributer offset from o define  
#define BMP_ATT_LEN				0x02	//file size, 4 bytes
#define BMP_ATT_DATA_OFFSET		0x0a	//offsset of the data, 4 bytes
#define BMP_ATT_WIDTH			0x12	//4//4  bytes
//值为正数时，图为倒向的；负数时，图为正向的
#define BMP_ATT_HEIGHT			0x16	//4  //4 bytes


/*文件相关*/
#define _os_file_handle									FILE *
#define _os_file_open(fd, name, id, mode) \
{ \
	int_8 str[32]; \
	if(id) \
	sprintf(str, "%s%d", name, id); \
	else \
	strcpy(str, name); \
	fd = fopen(str, mode); \
}
#define _os_file_close(fd)								fclose(fd)
#define _os_file_read(fd, buf, size, block)				fread(buf, block, size, fd)
#define _os_file_write(fd, buf, size, block)			fwrite(buf, block, size, fd)


/* bmp function relevant */
uint_16 bmp_get_attribute(
			char *fileName,  uint_32 *fileDataSize,  uint_32 *fileW,  uint_32 *fileH, uint_32 *offSet)
{
	_os_file_handle	handle= 0;
	uint_8			fileBuf[48];//file head, all attribute in it
	size_t			len;
	uint_32			fileSize=0;
	uint_32			dataOffset=0;
	int_32			vFileH=0;
	
	_os_file_open(handle, fileName, 0, "rb");
	if (handle==0) return -1;
	len = _os_file_read(handle, fileBuf, sizeof(fileBuf), sizeof(int_8));
	if (len!= sizeof(fileBuf)*sizeof(int_8)) 
	{
		_os_file_close(handle);
		return -1;
	}
	_os_file_close(handle);
	fileSize = MAKEDWORD(MAKEWORD(fileBuf[BMP_ATT_LEN], fileBuf[BMP_ATT_LEN+1]), 
						MAKEWORD(fileBuf[BMP_ATT_LEN+2], fileBuf[BMP_ATT_LEN+3]));
		
	dataOffset =  MAKEDWORD(MAKEWORD(fileBuf[BMP_ATT_DATA_OFFSET], fileBuf[BMP_ATT_DATA_OFFSET+1]), 
							MAKEWORD(fileBuf[BMP_ATT_DATA_OFFSET+2], fileBuf[BMP_ATT_DATA_OFFSET+3]));
	if (offSet)
		*offSet = dataOffset;
	if (fileDataSize)
		*fileDataSize = fileSize - dataOffset;
	if (fileW)
		*fileW = MAKEDWORD(MAKEWORD(fileBuf[BMP_ATT_WIDTH], fileBuf[BMP_ATT_WIDTH+1]), 
						MAKEWORD(fileBuf[BMP_ATT_WIDTH+2], fileBuf[BMP_ATT_WIDTH+3]));
	if (fileH)
		*fileH = MAKEDWORD(MAKEWORD(fileBuf[BMP_ATT_HEIGHT], fileBuf[BMP_ATT_HEIGHT+1]), 
						MAKEWORD(fileBuf[BMP_ATT_HEIGHT+2], fileBuf[BMP_ATT_HEIGHT+3]));
	
	return 0;
}

//MMI显示的黑白同MSPaint画出来的是相关的，故在此处预先将对应的值取反
//同时按PegBMP的要求，只需要单字节的补齐，不需要4字节的对齐存储，故将因4字节对齐的字节去掉
static uint_8 BMP_MASK[8] = {0, 0x80, 0xc0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
static uint_16 _bmp_covert_to_MMI_format(int_8 *buf, uint_32 fileWBytes, uint_32 fileWBits)
{
	int_8 	*pos = buf;
	uint_32	allReverseBytes = fileWBits/8;//不含填充位的有效字节
	uint_32	partReverseBytes = fileWBytes-allReverseBytes;//含填充位的字节数
	uint_8	padBit = fileWBits%8;//单字节中有效的bit数
	int		i;
	uint_8	spareBytes=0;
	
	//先整理没有填充bit的字节
	for (i=0; i<allReverseBytes; i++)
	{
		*pos++ = ~(*pos);
	}
	//有填充字节的字节
	for (i=0; i<partReverseBytes; i++)
	{
		//填充的第一字节，部分为有效的bit，有效的部分需取反
		if (i==0 && fileWBits%8)
		{
			*pos = ~(*pos);
			*pos++ = (*pos) & BMP_MASK[padBit];
		}
		else 
		{	//这些字节全为填充字节，强制为0，让MMI界面显示为白色
			spareBytes++;
		}
	}	

	return spareBytes;
}

//先计算每行补齐的多余字节
static uint_32 _bmp_recalc_file_len(uint_32 fileW, uint_32 fileH, uint_32 fileSize, 
										uint_32 fileDataSize, uint_16 *endPadBytes)
{
	uint_32	fileWBytes = ((fileW+31)/32)*4;//含补齐的每行字节数
	uint_32	allRealDataBytes = (fileW+7)/8;//不含补齐字节的每行字节数(含补齐bit的字节)
	uint_32 	spareBytes=0;
	uint_32	resultSize;
	
	spareBytes = fileWBytes - allRealDataBytes;
	resultSize = fileSize -spareBytes*fileH;
	fileDataSize -= spareBytes*fileH;
	if (fileDataSize%4)//不是4的倍数，强制对齐，Flash写入时按4字节对齐
	{
		*endPadBytes = 4-(fileDataSize%4);
		resultSize += *endPadBytes;
	}

	return resultSize;
}

static _write_to_bmp_c(_os_file_handle handle_c, int_8 *fileBuf, uint_32 dataOffset)
{
	char	buf[100];
	int_8	*pos = fileBuf;
	int		i;
	static  allLen=0;

	for (i=0; i<dataOffset; i++)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "0x%02x,", (uint_8)(*pos++));
		_os_file_write(handle_c, buf, 5, 1);
		allLen++;
		if (allLen%10==0)
		{
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "\n");
			_os_file_write(handle_c, buf, 1, 1);
		}
	}
}

//实际的位图中，高度分正负:正时表示位图倒向，负数表示位图正向
//将倒向存储的数据转为正向存储的数据
//成功返回0， 否则为负数
static int_8 _bmp_convert_data_dir(char *fileName)
{
	_os_file_handle	handle= 0;
	_os_file_handle 	newHandle = 0;
	int_8			*fileBuf;
	size_t			fileLen;
	size_t			wLen;
	uint_32			fileWBytes;
	uint_32			realWLen;
	uint_32			rowCount;
	int_8			*pos;
	uint_16			spareBytes=0;
	uint_16			endPadBytes =0;
	//获得相关属性的变量
	uint_32			fileSize = 0;
	uint_32			fileDataSize=0;
	uint_32			fileW = 0;
	uint_32			fileH = 0;
	int_32			vFileH = 0;
	uint_32			dataOffset = 0;
	//生成.c文件
	_os_file_handle	handle_c= 0;
	
	if (bmp_get_attribute(fileName, &fileDataSize, &fileW, &fileH, &dataOffset)!=0)
	{
		return -1;
	}
	vFileH = (int_32) fileH;
	if (vFileH<0) return 0;//文件数据已经为正向存储，不需要转换，直接返回成功
		
	_os_file_open(handle, fileName, 0, "rb");
	_os_file_open(handle_c, "bmp.c", 0, "w");
	if (handle==0) return -1;
	//文件内容可能较大，从堆上临时申请空间
	fileLen  = fileDataSize+dataOffset;
	fileBuf = (int_8 *)malloc(fileLen);
	memset(fileBuf, 0, fileDataSize);
	//获取原文件内容
	fileLen = _os_file_read(handle, fileBuf, fileLen, sizeof(int_8));
	_os_file_close(handle);
	if (fileLen!=fileLen) 
	{
		free(fileBuf);
		return -1;
	}
	//清空源BMP内容	
	_os_file_open(newHandle, "new.bmp", 0, "wb");
	if (newHandle==0) 
	{	
		free(fileBuf);
		return -1;
	}
	//写入文件头
	//重新计算文件长度，去掉win7下4字节补齐的多余字节
	fileSize = fileDataSize + dataOffset;
	fileSize = _bmp_recalc_file_len(fileW, fileH, fileSize, fileDataSize, &endPadBytes);
	fileBuf[BMP_ATT_LEN] = fileSize & 0xff;
	fileBuf[BMP_ATT_LEN+1] = (fileSize>>8) & 0xff;
	fileBuf[BMP_ATT_LEN+2] = (fileSize>>16) & 0xff;
	fileBuf[BMP_ATT_LEN+3] = (fileSize>>24) & 0xff;
	//设置高度为负数
	vFileH *= -1;//取反
	fileBuf[BMP_ATT_HEIGHT] = vFileH & 0xff;
	fileBuf[BMP_ATT_HEIGHT+1] = (vFileH>>8) & 0xff;
	fileBuf[BMP_ATT_HEIGHT+2] = (vFileH>>16) & 0xff;
	fileBuf[BMP_ATT_HEIGHT+3] = (vFileH>>24) & 0xff;
	wLen = _os_file_write(newHandle, fileBuf, dataOffset, sizeof(int_8));
	_write_to_bmp_c(handle_c, fileBuf, dataOffset, sizeof(int_8));
	
	if (wLen != dataOffset)
	{
		_os_file_close(newHandle);
		free(fileBuf);
		return -1;
	}
	//按正向写入文件内容
	fileWBytes = ((fileW+31)/32)*4;//每4个字节对齐
	for (rowCount=fileH; rowCount>0; rowCount--)
	{
		pos =(int_8*)(fileBuf+dataOffset+(rowCount-1)*fileWBytes);
		spareBytes = _bmp_covert_to_MMI_format(pos, fileWBytes, fileW); //CRITICAL
		realWLen =fileWBytes - spareBytes;
		if (rowCount==1)//最后一行,强制所有byte，4字节补齐
			realWLen += endPadBytes;
		wLen = _os_file_write(newHandle, pos, realWLen, sizeof(int_8));
		_write_to_bmp_c(handle_c, pos, realWLen);
		if (wLen != realWLen)
		{
			_os_file_close(newHandle);
			free(fileBuf);
			return -1;
		}
	}
	
	_os_file_close(newHandle);
	_os_file_close(handle_c);
	free(fileBuf);
	return 0;	
}

uint_16 bmp_get_info(char *fileName)
{
	uint_32	fileDataSize=0;
	uint_32	fileW = 0;
	uint_32	fileH = 0;
	int_32	vFileH = 0;//实际的位图中，高度分正负:正时表示位图倒向，负数表示位图正向
	uint_32	dataOffset = 0;

	printf("=================File %s=======================\n", fileName);
	if (bmp_get_attribute(fileName, &fileDataSize, &fileW, &fileH, &dataOffset)==0)
	{
		vFileH = (int_32) fileH;
		//如果位图为倒向存储，转为正向存储
		if (vFileH>0) 
		{	
			if (_bmp_convert_data_dir(fileName))
				return 0;//失败
			//转换完后，文件长度改变，需再重新读取一次属性
			if (bmp_get_attribute(fileName, &fileDataSize, &fileW, &fileH, &dataOffset)!=0)
				return 0; //失败
		}
		else 
			printf("The file had converted, do not convert\n");
	}
	printf("Size:%d\n", fileDataSize+dataOffset);
	printf("fileDataSize:%d\n", fileDataSize);
	printf("Width:%d\n", fileW);
	printf("Height:%d\n", fileH);
	scanf("%d", &fileH);
}
