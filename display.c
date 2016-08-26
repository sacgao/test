#include <stdio.h>

/*�������Ͷ���*/
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
//ֵΪ����ʱ��ͼΪ����ģ�����ʱ��ͼΪ�����
#define BMP_ATT_HEIGHT			0x16	//4  //4 bytes


/*�ļ����*/
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

//MMI��ʾ�ĺڰ�ͬMSPaint������������صģ����ڴ˴�Ԥ�Ƚ���Ӧ��ֵȡ��
//ͬʱ��PegBMP��Ҫ��ֻ��Ҫ���ֽڵĲ��룬����Ҫ4�ֽڵĶ���洢���ʽ���4�ֽڶ�����ֽ�ȥ��
static uint_8 BMP_MASK[8] = {0, 0x80, 0xc0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
static uint_16 _bmp_covert_to_MMI_format(int_8 *buf, uint_32 fileWBytes, uint_32 fileWBits)
{
	int_8 	*pos = buf;
	uint_32	allReverseBytes = fileWBits/8;//�������λ����Ч�ֽ�
	uint_32	partReverseBytes = fileWBytes-allReverseBytes;//�����λ���ֽ���
	uint_8	padBit = fileWBits%8;//���ֽ�����Ч��bit��
	int		i;
	uint_8	spareBytes=0;
	
	//������û�����bit���ֽ�
	for (i=0; i<allReverseBytes; i++)
	{
		*pos++ = ~(*pos);
	}
	//������ֽڵ��ֽ�
	for (i=0; i<partReverseBytes; i++)
	{
		//���ĵ�һ�ֽڣ�����Ϊ��Ч��bit����Ч�Ĳ�����ȡ��
		if (i==0 && fileWBits%8)
		{
			*pos = ~(*pos);
			*pos++ = (*pos) & BMP_MASK[padBit];
		}
		else 
		{	//��Щ�ֽ�ȫΪ����ֽڣ�ǿ��Ϊ0����MMI������ʾΪ��ɫ
			spareBytes++;
		}
	}	

	return spareBytes;
}

//�ȼ���ÿ�в���Ķ����ֽ�
static uint_32 _bmp_recalc_file_len(uint_32 fileW, uint_32 fileH, uint_32 fileSize, 
										uint_32 fileDataSize, uint_16 *endPadBytes)
{
	uint_32	fileWBytes = ((fileW+31)/32)*4;//�������ÿ���ֽ���
	uint_32	allRealDataBytes = (fileW+7)/8;//���������ֽڵ�ÿ���ֽ���(������bit���ֽ�)
	uint_32 	spareBytes=0;
	uint_32	resultSize;
	
	spareBytes = fileWBytes - allRealDataBytes;
	resultSize = fileSize -spareBytes*fileH;
	fileDataSize -= spareBytes*fileH;
	if (fileDataSize%4)//����4�ı�����ǿ�ƶ��룬Flashд��ʱ��4�ֽڶ���
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

//ʵ�ʵ�λͼ�У��߶ȷ�����:��ʱ��ʾλͼ���򣬸�����ʾλͼ����
//������洢������תΪ����洢������
//�ɹ�����0�� ����Ϊ����
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
	//���������Եı���
	uint_32			fileSize = 0;
	uint_32			fileDataSize=0;
	uint_32			fileW = 0;
	uint_32			fileH = 0;
	int_32			vFileH = 0;
	uint_32			dataOffset = 0;
	//����.c�ļ�
	_os_file_handle	handle_c= 0;
	
	if (bmp_get_attribute(fileName, &fileDataSize, &fileW, &fileH, &dataOffset)!=0)
	{
		return -1;
	}
	vFileH = (int_32) fileH;
	if (vFileH<0) return 0;//�ļ������Ѿ�Ϊ����洢������Ҫת����ֱ�ӷ��سɹ�
		
	_os_file_open(handle, fileName, 0, "rb");
	_os_file_open(handle_c, "bmp.c", 0, "w");
	if (handle==0) return -1;
	//�ļ����ݿ��ܽϴ󣬴Ӷ�����ʱ����ռ�
	fileLen  = fileDataSize+dataOffset;
	fileBuf = (int_8 *)malloc(fileLen);
	memset(fileBuf, 0, fileDataSize);
	//��ȡԭ�ļ�����
	fileLen = _os_file_read(handle, fileBuf, fileLen, sizeof(int_8));
	_os_file_close(handle);
	if (fileLen!=fileLen) 
	{
		free(fileBuf);
		return -1;
	}
	//���ԴBMP����	
	_os_file_open(newHandle, "new.bmp", 0, "wb");
	if (newHandle==0) 
	{	
		free(fileBuf);
		return -1;
	}
	//д���ļ�ͷ
	//���¼����ļ����ȣ�ȥ��win7��4�ֽڲ���Ķ����ֽ�
	fileSize = fileDataSize + dataOffset;
	fileSize = _bmp_recalc_file_len(fileW, fileH, fileSize, fileDataSize, &endPadBytes);
	fileBuf[BMP_ATT_LEN] = fileSize & 0xff;
	fileBuf[BMP_ATT_LEN+1] = (fileSize>>8) & 0xff;
	fileBuf[BMP_ATT_LEN+2] = (fileSize>>16) & 0xff;
	fileBuf[BMP_ATT_LEN+3] = (fileSize>>24) & 0xff;
	//���ø߶�Ϊ����
	vFileH *= -1;//ȡ��
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
	//������д���ļ�����
	fileWBytes = ((fileW+31)/32)*4;//ÿ4���ֽڶ���
	for (rowCount=fileH; rowCount>0; rowCount--)
	{
		pos =(int_8*)(fileBuf+dataOffset+(rowCount-1)*fileWBytes);
		spareBytes = _bmp_covert_to_MMI_format(pos, fileWBytes, fileW); //CRITICAL
		realWLen =fileWBytes - spareBytes;
		if (rowCount==1)//���һ��,ǿ������byte��4�ֽڲ���
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
	int_32	vFileH = 0;//ʵ�ʵ�λͼ�У��߶ȷ�����:��ʱ��ʾλͼ���򣬸�����ʾλͼ����
	uint_32	dataOffset = 0;

	printf("=================File %s=======================\n", fileName);
	if (bmp_get_attribute(fileName, &fileDataSize, &fileW, &fileH, &dataOffset)==0)
	{
		vFileH = (int_32) fileH;
		//���λͼΪ����洢��תΪ����洢
		if (vFileH>0) 
		{	
			if (_bmp_convert_data_dir(fileName))
				return 0;//ʧ��
			//ת������ļ����ȸı䣬�������¶�ȡһ������
			if (bmp_get_attribute(fileName, &fileDataSize, &fileW, &fileH, &dataOffset)!=0)
				return 0; //ʧ��
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
