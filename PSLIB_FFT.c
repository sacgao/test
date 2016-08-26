/**
 *	FFT - Fast Fourier transform. The length of X must be a power 
 *	of two, for a fast radix-2 fast-Fourier transform algorithm 
 *	is used.	spadger@bmy <echo.xjtu@gmail.com> 2007.9.2
 */

#include <math.h>
#include <stdio.h>

#include "PSLIB_FFT.h"



#define MY_TIMER1	((TIMERELAY *)(pelm->apvUser[0])) /*用户指针0被分配到时间继电器*/
#define MY_TICK		((uint32_t)(pelm->aulUser[0]))
#define MY_FLAG1	        ((uint32_t)(pelm->aulUser[1]))
//#define MY_DFT	        ((COMPLEX_MY *)(pelm->apvUser[1]))

/*标准系数应该是除以1.414，
但是由于硬件低通的关系，
基波幅值将减少，所以系数
要放大*/
#define Coef_h0 (0.70710678/1)
#define Coef_h1 (0.047816)//(0.0454252)// (0.70710678/0.9729)*2/SAMFREQ32   
#define Coef_h2 (0.045323)//(0.70710678/0.9304) 
#define Coef_h3 (0.045349)//(0.70710678/0.8699) 
#define Coef_h4 (0.045398)//(0.70710678/0.8008) 
#define Coef_h5 (0.045346)//(0.70710678/0.7318)   
#define Coef_h6 (0.045415)//(0.70710678/0.6666)   
#define Coef_h7 (0.045410)//(0.70710678/0.6077)   
#define Coef_h8 (0.045425)//(0.70710678/0.5542)   
#define Coef_h9 (0.045425)//(0.70710678/0.5082)   
#define Coef_h10 (0.045425)//(0.70710678/0.4667)   
#define Coef_h11 (0.04542)//(0.70710678/0.4308)   
#define Coef_h12 (0.04542)//(0.70710678/0.410)   
#define Coef_h13 (0.04542)//(0.70710678/0.365)   
#define Coef_h14 (0.0454)//(0.70710678/0.339)   
#define Coef_h15 (0.0454)//(0.70710678/0.330)   
#define Coef_h16 (0.0454)//(0.70710678/0.290)   

typedef struct
{
	float r;
	float i;
}COMPLEX_MY;
typedef COMPLEX_MY  cplx_t;

//求复数的幅值
#define F_AMP_MY(xVal)	(sqrt((xVal.r)*(xVal.r)+(xVal.i)*(xVal.i)))


const float cos1=1;
const float sin1=0;
const float cos2[2]={1,0};
const float sin2[2]={0,-1};
const float cos4[4]={1.000000,0.707107,0.000000,-0.707107};
const float sin4[4]={0.000000,-0.707107,-1.000000,-0.707107};
const float cos8[8]={1.000000,0.923880,0.707107,0.382683,0.000000,-0.382683,-0.707107,-0.923880};
const float sin8[8]={0.000000,-0.382683,-0.707107,-0.923880,-1.000000,-0.923880,-0.707107,-0.382683};
const float cos16[16]={1.000000, 0.980785,0.923880,0.831470,0.707107,0.555570,0.382683,0.195090,0.000000,-0.195090,-0.382683,-0.555570,-0.707107,-0.831470,-0.923880,-0.980785};
const float sin16[16]={0.000000, -0.195090,-0.382683,-0.555570,-0.707107,-0.831470,-0.923880,-0.980785,-1.000000,-0.980785,-0.923880,-0.831470,-0.707107,-0.555570,-0.382683,-0.195090};



void cplx_mul(cplx_t *x, cplx_t *y, cplx_t *r)
{
	r->r=x->r*y->r-x->i*y->i;
	r->i=x->r*y->i+x->i*y->r;
}

void cplx_exp(cplx_t *x, cplx_t *r)
{
	double expx=exp(x->r);	
	r->r=expx*cos(x->i);
	r->i=expx*sin(x->i);
}

void W_exp_32(int l ,int k , cplx_t *r)
{
	switch(l)
	{ 
		case 1 :
          r->r=cos1;
		  r->i=sin1;
        break;
 
        case 2:
          r->r=cos2[k];
		  r->i=sin2[k];
        break;

		case 4:
          r->r=cos4[k];
		  r->i=sin4[k];
        break;

		case 8:
          r->r=cos8[k];
		  r->i=sin8[k];
        break;

		case 16:
          r->r=cos16[k];
		  r->i=sin16[k];
        break;

	}

}


void bit_reverse(cplx_t *x, unsigned int N)
{
	double t;
	cplx_t tmp;
	unsigned int i=0,j=0,k=0;
	for(i=0; i<N; i++) 
	{
		k=i;
		j=0;
		t=log(0.0+N)/log(2.0);
		while((t--)>0) 
		{
			j<<=1;
			j|=k&1;
			k>>=1;
		}
		if(j>i) 
		{
			tmp=x[i];
			x[i]=x[j];
			x[j]=tmp;
		}
	}
}

void fft(cplx_t *x, int N)
{
	cplx_t u,d,p,W;
	int i=0,j=0,k=0,l=0;
    int M=floor(log(0.0+N)/log(2.0));
    int MM=log(0.0+N)/log(2.0);
	
	if((MM-M) > 0)
	{
		printf("The length of x (N) must be a power of two!!!\n");
		return;
	}
	bit_reverse(x,N);
	for(i=0; i<M; i++) 
	{
		l=1<<i;
		for(j=0; j<N; j+=2*l ) 
		{
			for(k=0; k<l; k++) 
			{
				//tmp.r=0.0;
				//tmp.i=-2*M_PI*k/2/l;
				//cplx_exp(&tmp,&W);
				W_exp_32( l , k , &W);
				cplx_mul(&x[j+k+l],&W,&p);
				u.r=x[j+k].r+p.r;
				u.i=x[j+k].i+p.i;
				d.r=x[j+k].r-p.r;
				d.i=x[j+k].i-p.i;
				x[j+k]=u;
				x[j+k+l]=d;
			}
		}
	}
}



void fft2( cplx_t* x, cplx_t* y,unsigned int len)
{
        unsigned int ex=0,t=len;
        unsigned int i,j,k;
        cplx_t  w;
        float tr,ti,rr,ri,yr,yi;
        
       // if(NOT2POW(len)) return NULL;        //如果失败，返回空指针
        for(;!(t&1);t>>=1) ex++;        //len应该等于2的ex次方
        
        //y=(my_complex*)malloc(len*sizeof(my_complex));
        //if(!y) return NULL;
        
        //变址计算，库里-图基算法
        for(i=0;i<len;i++)
		{
			k=i;
			j=0;
			t=ex;
			while((t--)>0)
			{
				j<<=1;
				j|=k&1;
				k>>=1;
			}
			if(j>=i)
			{
				y[i]=x[j];
				y[j]=x[i];
			}
		}
        //用变址后的y向量进行计算
        for(i=0;i<ex;i++)
		{
			t=1<<i;
			for(j=0;j<len;j+=t<<1)
			{
				for(k=0;k<t;k++)
				{
					//ti=-M_PI*k/t;
					//rr=cos(ti);
					//ri=sin(ti);
					W_exp_32( t , k , &w);
					rr=w.r;
					ri=w.i;
                                
					tr=y[j+k+t].r;
					ti=y[j+k+t].i;
                                
					yr=rr*tr-ri*ti;
					yi=rr*ti+ri*tr;
                                
					tr=y[j+k].r;
					ti=y[j+k].i;
                                
					y[j+k].r=tr+yr;
					y[j+k].i=ti+yi;
					y[j+k+t].r=tr-yr;
					y[j+k+t].i=ti-yi;
				}
			}
        }
        
       // return y;
}



void fft_split(cplx_t *a, cplx_t *x, cplx_t *y, unsigned int N)
{
	unsigned int i;

	x[0].r=a[0].r;
	x[0].i=0;

	y[0].r=a[0].i;
	y[0].i=0;
	for (i=1;i<N;i++)
	{
		x[i].r=(a[i].r+a[N-i].r)/2.0;
        x[i].i=(a[i].i-a[N-i].i)/2.0;

		y[i].r=(a[i].i+a[N-i].i)/2.0;
        y[i].i=(a[N-i].r-a[i].r)/2.0;
	}
}


/* 输入信号*/
enum 
{_ENABLE =0, _IN1=1, _IN2=2};
//{_ENABLE =0, _IN1=1};


/* 输出信号*/
enum 
{_AMP1=0,_AMP2,_AMP3,_AMP4,_AMP5,_AMP6,_AMP7,_AMP8,_AMP9,_AMP10,_AMP11,_AMP12,_AMP13,_AMP14,_RMS,_THD=15,_AMP1_1,_AMP2_1,_AMP3_1,_AMP4_1,_AMP5_1,_AMP6_1,_AMP7_1,_AMP8_1,_AMP9_1,_AMP10_1,_AMP11_1,_AMP12_1,_AMP13_1,_AMP14_1,_RMS_1,_THD_1=31};

static void PSLIB_FFT_Scan(EP_ELEMENT *pelm)
{  
	float value_a;
	uint32_t ulSamCnt;
	float *pfChTemp,*pfChTemp2;		
	int j;
    BOOL bfinished;
	COMPLEX_MY fSam[SAMFREQ32],fSam_out[SAMFREQ32],fSam1[SAMFREQ32];  
	
       //#if   PSLIB_FFT_Enabled 
	//return;
       //#endif		
	if (pelm->pchart->bSetChg)
	{
		
	}
	if (!pelm->ppioIn[_ENABLE]->now.bVal)
	{
		//ResetTR(MY_TIMER1);
	//	pelm->aioOut[_FINISHED].now.bVal=FALSE;
	//	pelm->aioOut[_AMP0].now.fVal =0.0;
		pelm->aioOut[_AMP1].now.fVal =0.0;	
		pelm->aioOut[_AMP2].now.fVal =0.0;	
		pelm->aioOut[_AMP3].now.fVal =0.0;			
		pelm->aioOut[_AMP4].now.fVal =0.0;			
		pelm->aioOut[_AMP5].now.fVal =0.0;	
		pelm->aioOut[_AMP6].now.fVal =0.0;	
		pelm->aioOut[_AMP7].now.fVal =0.0;	
		pelm->aioOut[_AMP8].now.fVal =0.0;	
		pelm->aioOut[_AMP9].now.fVal =0.0;	
		pelm->aioOut[_AMP10].now.fVal =0.0;	
		pelm->aioOut[_AMP11].now.fVal =0.0;
		pelm->aioOut[_AMP12].now.fVal =0.0;
		pelm->aioOut[_AMP13].now.fVal =0.0;
		pelm->aioOut[_AMP14].now.fVal =0.0;
		//pelm->aioOut[_AMP15].now.fVal =0.0;
        pelm->aioOut[_RMS].now.fVal=0.0;
        pelm->aioOut[_THD].now.fVal=0.0;

	//	pelm->aioOut[_AMP0_1].now.fVal =0.0;
		pelm->aioOut[_AMP1_1].now.fVal =0.0;	
		pelm->aioOut[_AMP2_1].now.fVal =0.0;	
		pelm->aioOut[_AMP3_1].now.fVal =0.0;			
		pelm->aioOut[_AMP4_1].now.fVal =0.0;			
		pelm->aioOut[_AMP5_1].now.fVal =0.0;	
		pelm->aioOut[_AMP6_1].now.fVal =0.0;	
		pelm->aioOut[_AMP7_1].now.fVal =0.0;	
		pelm->aioOut[_AMP8_1].now.fVal =0.0;	
		pelm->aioOut[_AMP9_1].now.fVal =0.0;	
		pelm->aioOut[_AMP10_1].now.fVal =0.0; 
		pelm->aioOut[_AMP11_1].now.fVal =0.0;
		pelm->aioOut[_AMP12_1].now.fVal =0.0;
		pelm->aioOut[_AMP13_1].now.fVal =0.0;
		pelm->aioOut[_AMP14_1].now.fVal =0.0;
		//pelm->aioOut[_AMP15_1].now.fVal =0.0;
		pelm->aioOut[_RMS_1].now.fVal=0.0;
		pelm->aioOut[_THD_1].now.fVal=0.0;


		return;
	 }
	
 	
    bfinished=FALSE;	
	ulSamCnt=pelm->pchart->ulScnAiCnt;  


	pfChTemp=(float *)RD_Lgc_AI_P(pelm->ppioIn[1]->pvCh,ulSamCnt);		
	pfChTemp2=(float *)RD_Lgc_AI_P(pelm->ppioIn[2]->pvCh,ulSamCnt);		

	for(j=0;j<SAMFREQ32;j++)
	{
		fSam[j].r=RD_Lgc_AI (pfChTemp, -SAMFREQ32+1+j);
        fSam[j].i=RD_Lgc_AI (pfChTemp2, -SAMFREQ32+1+j);
        //fSam[j].i=0;
	}
                    
		   				      	
	//fft(fSam,SAMFREQ32);
	fft2(fSam,fSam_out,SAMFREQ32);
	fft_split(fSam_out, fSam, fSam1, SAMFREQ32);
               
    bfinished=TRUE;		 
				
 
    // value_a=(2.0/SAMFREQ32)*Coef_h0*F_AMP_MY(fSam[0]);
	//pelm->aioOut[_AMP0].now.fVal =value_a;
			
	value_a=Coef_h1*F_AMP_MY(fSam[1]);	
	pelm->aioOut[_AMP1].now.fVal =value_a;	
		
	value_a=Coef_h2*F_AMP_MY(fSam[2]);	
	pelm->aioOut[_AMP2].now.fVal =value_a;
		
	value_a=Coef_h3*F_AMP_MY(fSam[3]);	
	pelm->aioOut[_AMP3].now.fVal =value_a;	
		
	value_a=Coef_h4*F_AMP_MY(fSam[4]);	
	pelm->aioOut[_AMP4].now.fVal =value_a;	
		
	value_a=Coef_h5*F_AMP_MY(fSam[5]);	
	pelm->aioOut[_AMP5].now.fVal =value_a;
		
	value_a=Coef_h6*F_AMP_MY(fSam[6]);	
	pelm->aioOut[_AMP6].now.fVal =value_a;	
			
	value_a=Coef_h7*F_AMP_MY(fSam[7]);	
	pelm->aioOut[_AMP7].now.fVal =value_a;	
		
	value_a=Coef_h8*F_AMP_MY(fSam[8]);	
	pelm->aioOut[_AMP8].now.fVal =value_a;	
		
	value_a=Coef_h9*F_AMP_MY(fSam[9]);	
	pelm->aioOut[_AMP9].now.fVal =value_a;	

	value_a=Coef_h10*F_AMP_MY(fSam[10]);	
	pelm->aioOut[_AMP10].now.fVal =value_a;	

	value_a=Coef_h11*F_AMP_MY(fSam[11]);	
	pelm->aioOut[_AMP11].now.fVal =value_a;

	value_a=Coef_h12*F_AMP_MY(fSam[12]);	
	pelm->aioOut[_AMP12].now.fVal =value_a;

	value_a=Coef_h13*F_AMP_MY(fSam[13]);	
	pelm->aioOut[_AMP13].now.fVal =value_a;

	value_a=Coef_h14*F_AMP_MY(fSam[14]);	
	pelm->aioOut[_AMP14].now.fVal =value_a;

	//value_a=(2.0/SAMFREQ32)*Coef_h15*F_AMP_MY(fSam[15]);	
	//pelm->aioOut[_AMP15].now.fVal =value_a;
        
    value_a=0;
    for(j=_AMP2;j<=_AMP14;j++)
    { 
		value_a=(pelm->aioOut[j].now.fVal) *(pelm->aioOut[j].now.fVal)+value_a ;
	}

	if (pelm->aioOut[_AMP1].now.fVal ==0)
		pelm->aioOut[_THD].now.fVal=0;
	else
		pelm->aioOut[_THD].now.fVal = (sqrt(value_a)/(pelm->aioOut[_AMP1].now.fVal ));
               
          
	    value_a=(pelm->aioOut[_AMP1].now.fVal) *(pelm->aioOut[_AMP1].now.fVal)+value_a ; 
	    value_a=  sqrt(value_a);
	    pelm->aioOut[_RMS].now.fVal =value_a;


		//value_a=(2.0/SAMFREQ32)*Coef_h0*F_AMP_MY(fSam1[0]);
		//pelm->aioOut[_AMP0_1].now.fVal =value_a;
				
		value_a=Coef_h1*F_AMP_MY(fSam1[1]);	
		pelm->aioOut[_AMP1_1].now.fVal =value_a;	
			
		value_a=Coef_h2*F_AMP_MY(fSam1[2]);	
		pelm->aioOut[_AMP2_1].now.fVal =value_a;
			
		value_a=Coef_h3*F_AMP_MY(fSam1[3]);	
		pelm->aioOut[_AMP3_1].now.fVal =value_a;	
			
		value_a=Coef_h4*F_AMP_MY(fSam1[4]);	
		pelm->aioOut[_AMP4_1].now.fVal =value_a;	
			
		value_a=Coef_h5*F_AMP_MY(fSam1[5]);	
		pelm->aioOut[_AMP5_1].now.fVal =value_a;
			
		value_a=Coef_h6*F_AMP_MY(fSam1[6]);	
		pelm->aioOut[_AMP6_1].now.fVal =value_a;	
				
		value_a=Coef_h7*F_AMP_MY(fSam1[7]);	
		pelm->aioOut[_AMP7_1].now.fVal =value_a;	
			
		value_a=Coef_h8*F_AMP_MY(fSam1[8]);	
		pelm->aioOut[_AMP8_1].now.fVal =value_a;	
			
		value_a=Coef_h9*F_AMP_MY(fSam1[9]);	
		pelm->aioOut[_AMP9_1].now.fVal =value_a;	
	
		value_a=Coef_h10*F_AMP_MY(fSam1[10]);	
		pelm->aioOut[_AMP10_1].now.fVal =value_a; 
	
		value_a=Coef_h11*F_AMP_MY(fSam1[11]);	
		pelm->aioOut[_AMP11_1].now.fVal =value_a;
	
		value_a=Coef_h12*F_AMP_MY(fSam1[12]);	
		pelm->aioOut[_AMP12_1].now.fVal =value_a;
	
		value_a=Coef_h13*F_AMP_MY(fSam1[13]);	
		pelm->aioOut[_AMP13_1].now.fVal =value_a;
	
		value_a=Coef_h14*F_AMP_MY(fSam1[14]);	
		pelm->aioOut[_AMP14_1].now.fVal =value_a;
	
		//value_a=(2.0/SAMFREQ32)*Coef_h15*F_AMP_MY(fSam1[15]);	
		//pelm->aioOut[_AMP15_1].now.fVal =value_a;
			
		value_a=0;
		for(j=_AMP2_1;j<=_AMP14_1;j++)
		{ 
			value_a=(pelm->aioOut[j].now.fVal) *(pelm->aioOut[j].now.fVal)+value_a ;
		}
	
		if (pelm->aioOut[_AMP1_1].now.fVal ==0)
			pelm->aioOut[_THD_1].now.fVal=0;
		else
			pelm->aioOut[_THD_1].now.fVal = (sqrt(value_a)/(pelm->aioOut[_AMP1_1].now.fVal ));
				   
			  
		value_a=(pelm->aioOut[_AMP1_1].now.fVal) *(pelm->aioOut[_AMP1_1].now.fVal)+value_a ; 
		value_a=  sqrt(value_a);
		pelm->aioOut[_RMS_1].now.fVal =value_a;

		// pelm->aioOut[_FINISHED].now.bVal=bfinished;
}

EP_STATUS PSLIB_FFT(EP_ELEMENT *pelm)
{

	MY_FLAG1  =0;
	MY_TICK   =TM_Get_usCnt();	
	//MY_TIMER1 =(TIMERELAY *)malloc(sizeof(TIMERELAY));
	//MY_DFT=(COMPLEX *)malloc(sizeof(COMPLEX)*HARMONIC_NUMBER);//

	
	
	//输出初始化
	//  pelm->aioOut[_FINISHED].now.bVal=FALSE;
	//pelm->aioOut[_AMP0].now.fVal =0.0;
	pelm->aioOut[_AMP1].now.fVal =0.0;	
	pelm->aioOut[_AMP2].now.fVal =0.0;	
	pelm->aioOut[_AMP3].now.fVal =0.0;			
	pelm->aioOut[_AMP4].now.fVal =0.0;			
	pelm->aioOut[_AMP5].now.fVal =0.0;	
	pelm->aioOut[_AMP6].now.fVal =0.0;	
	pelm->aioOut[_AMP7].now.fVal =0.0;	
	pelm->aioOut[_AMP8].now.fVal =0.0;	
	pelm->aioOut[_AMP9].now.fVal =0.0;
	pelm->aioOut[_AMP10].now.fVal =0.0;	
	pelm->aioOut[_AMP11].now.fVal =0.0;
	pelm->aioOut[_AMP12].now.fVal =0.0;
	pelm->aioOut[_AMP13].now.fVal =0.0;
	pelm->aioOut[_AMP14].now.fVal =0.0;
	//pelm->aioOut[_AMP15].now.fVal =0.0;
    pelm->aioOut[_RMS].now.fVal=0.0;
	pelm->aioOut[_THD].now.fVal=0.0;


	//pelm->aioOut[_AMP0_1].now.fVal =0.0;
	pelm->aioOut[_AMP1_1].now.fVal =0.0;	
	pelm->aioOut[_AMP2_1].now.fVal =0.0;	
	pelm->aioOut[_AMP3_1].now.fVal =0.0;			
	pelm->aioOut[_AMP4_1].now.fVal =0.0;			
	pelm->aioOut[_AMP5_1].now.fVal =0.0;	
	pelm->aioOut[_AMP6_1].now.fVal =0.0;	
	pelm->aioOut[_AMP7_1].now.fVal =0.0;	
	pelm->aioOut[_AMP8_1].now.fVal =0.0;	
	pelm->aioOut[_AMP9_1].now.fVal =0.0;	
	pelm->aioOut[_AMP10_1].now.fVal =0.0; 
	pelm->aioOut[_AMP11_1].now.fVal =0.0;
	pelm->aioOut[_AMP12_1].now.fVal =0.0;
	pelm->aioOut[_AMP13_1].now.fVal =0.0;
	pelm->aioOut[_AMP14_1].now.fVal =0.0;
	//pelm->aioOut[_AMP15_1].now.fVal =0.0;
	pelm->aioOut[_RMS_1].now.fVal=0.0;
	pelm->aioOut[_THD_1].now.fVal=0.0;

	
	pelm->Scan_Func=PSLIB_FFT_Scan;
	
	return EP_SUCCESS;	
}


/**
 *	for test and demonstation, set '#if 0' to comment this out.
 */  
#if 1
#define DATA_LEN 64
int main()
{
	int i;
	cplx_t x[DATA_LEN];
	for(i=0;i<DATA_LEN;i++){
		x[i].r=i;
		x[i].i=i-1;
	}
	
	printf("Before...\nReal\t\tImag\n");
	for(i=0;i<DATA_LEN;i++)
		printf("%f\t%f\n",x[i].r,x[i].i);

	//cplx_exp(&x[1],&x[0]);
	//bit_reverse(x,8);
	fft(x,DATA_LEN);

	printf("After...\nReal\t\tImag\n");
	for(i=0;i<DATA_LEN;i++)
		printf("%f\t%f\n",x[i].r,x[i].i);

	return 0;
}
#endif
