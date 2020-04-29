#include "sccb.h"
#include "esp_mfi_i2c.h"


//初始化SCCB接口 
void SCCB_Init(void)
{											      	 
	esp_mfi_i2c_init();
}			 

//写寄存器
//返回值:0,成功;1,失败.
uint8_t SCCB_WR_Reg(uint8_t reg, uint8_t data)
{
    int ret = -1;
    ret = esp_mfi_i2c_write(SCCB_ID, reg, &data, 1);

    return	(ret == 0) ? 0 : 1;
}
//读寄存器
//返回值:读到的寄存器值
uint8_t SCCB_RD_Reg(uint8_t reg)
{
    uint8_t val = 0;
    int ret = -1;

    ret = esp_mfi_i2c_read(SCCB_ID, reg, &val, 1);

    if (ret != 0) {
        printf("SCCB_RD_Reg error\n");
    }

    return val;
}
















