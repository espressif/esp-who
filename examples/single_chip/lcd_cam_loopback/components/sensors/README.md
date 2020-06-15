```c
#include "ov2640.h" 
 
uint8_t ov2640_mode=0;						//工作模式:0,RGB565模式;1,JPEG模式

//JPEG尺寸支持列表
const uint16_t jpeg_img_size_tbl[][2]=
{
	160,120,	//QQVGA
	176,144,	//QCIF
	320,240,	//QVGA
	400,240,	//WQVGA
	352,288,	//CIF
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,800,	//WXGA
	1280,960,	//XVGA
	1440,900,	//WXGA+
	1280,1024,	//SXGA
	1600,1200,	//UXGA	
};
const uint8_t*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
const uint8_t*JPEG_SIZE_TBL[13]={"QQVGA","QCIF","QVGA","WQVGA","CIF","VGA","SVGA","XGA","WXGA","XVGA","WXGA+","SXGA","UXGA"};//JPEG图片 13种尺寸 

//JPEG测试
//JPEG数据,通过串口2发送给电脑.
void jpeg_test(void)
{
 	OV2640_JPEG_Mode();		//JPEG模式 
	OV2640_OutSize_Set(jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸    
} 

//RGB565测试
//RGB数据直接显示在LCD上面
void rgb565_test(void)
{ 
	OV2640_RGB565_Mode();	//RGB565模式
  	OV2640_OutSize_Set(lcddev.width,lcddev.height); 
} 
```