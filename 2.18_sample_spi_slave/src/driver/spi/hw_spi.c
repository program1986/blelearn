#include "board.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>

#include "hw_spi.h"
#include "hw_uart.h"
/*********************************************************************
 * LOCAL PARAMETER
 */
SPI_Handle      SPIHandle;
SPI_Params      SPIparams;

SPI_Transaction spiTransaction;
/*********************************************************************
 * LOCAL FUNCTIONS
 */

void spiSlaveCallback(SPI_Handle handle,SPI_Transaction * transaction)
{
   HwUARTWrite(spiTransaction.rxBuf, 4);
   SPI_close(SPIHandle);
}
/*********************************************************************
 * @fn      GY_SpiTask_Init
 *
 * @brief   SPI�����ʼ��������
 *
 * @param   spiIndex -> 0:Board_SPI0 | 1:Board_SPI1
 *          spiMode ->  0:SPI_MASTER | 1:SPI_SLAVE
 *
 * @return  None.
 */
void HwSPIInit(void)
{
  SPI_init();
  SPI_Params_init(&SPIparams);
  SPIparams.bitRate  = 1000000;                    //1MHz
  SPIparams.dataSize = 8; 
  SPIparams.frameFormat = SPI_POL0_PHA0;           //��λ0����0
  SPIparams.mode = SPI_SLAVE;                      //SPI����ģʽ
  SPIparams.transferMode = SPI_MODE_CALLBACK;      //�ص�ģʽ
  SPIparams.transferCallbackFxn = spiSlaveCallback;//�ص�����
  SPIparams.transferTimeout = SPI_WAIT_FOREVER;  
}

/*********************************************************************
 * @fn      GY_SPI_Trans
 *
 * @brief   SPIͨ�ź���
 *
 * @param   csnPin -> CSN����
 *          txbuf -> ��������
 *          txbuf -> ��������
 *          len -> ͨ�ŵ����ݳ���
 *
 * @return  None.
 */
void HwSPITrans(uint8_t csnPin, uint8_t *txbuf, uint8_t *rxbuf ,uint16_t len)
{
  spiTransaction.arg = NULL;
  spiTransaction.count = len;
  spiTransaction.txBuf = txbuf;
  spiTransaction.rxBuf = rxbuf;
  
  SPIHandle = SPI_open(CC2640R2_LAUNCHXL_SPI0, &SPIparams); 
  int csnFlag;
  csnFlag = SPI_control(SPIHandle, SPICC26XXDMA_CMD_SET_CSN_PIN, &csnPin);  //ѡ��CSNƬѡ����
  if(csnFlag == SPI_STATUS_SUCCESS)
  {
    SPI_transfer(SPIHandle, &spiTransaction);
  }
}


