/*
  ***************************************************************************************************************
  File:       NRF24L01.c
  Config:     EN_AA = 0 (no ACK), Fixed payload 32 bytes, Channel 10
              TX Address = RX Address = {0xEE,0xDD,0xCC,0xBB,0xAA}
  ***************************************************************************************************************
*/

#include "stm32f1xx_hal.h"
#include "NRF24L01.h"

extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI &hspi1

#define NRF24_CE_PORT    GPIOA
#define NRF24_CE_PIN     GPIO_PIN_3
#define NRF24_CSN_PORT   GPIOA
#define NRF24_CSN_PIN    GPIO_PIN_4

/* ------------------------------------------------------------------ */
/*  GPIO helpers                                                        */
/* ------------------------------------------------------------------ */
void CS_Select(void)   { HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_RESET); }
void CS_UnSelect(void) { HAL_GPIO_WritePin(NRF24_CSN_PORT, NRF24_CSN_PIN, GPIO_PIN_SET);   }
void CE_Enable(void)   { HAL_GPIO_WritePin(NRF24_CE_PORT,  NRF24_CE_PIN,  GPIO_PIN_SET);   }
void CE_Disable(void)  { HAL_GPIO_WritePin(NRF24_CE_PORT,  NRF24_CE_PIN,  GPIO_PIN_RESET); }

/* ------------------------------------------------------------------ */
/*  Low-level SPI                                                       */
/* ------------------------------------------------------------------ */
void nrf24_WriteReg(uint8_t Reg, uint8_t Data)
{
    uint8_t buf[2] = { Reg | 0x20, Data };
    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);
    CS_UnSelect();
}

void nrf24_WriteRegMulti(uint8_t Reg, uint8_t *data, int size)
{
    uint8_t cmd = Reg | 0x20;
    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &cmd,  1,    100);
    HAL_SPI_Transmit(NRF24_SPI, data,  size, 1000);
    CS_UnSelect();
}

uint8_t nrf24_ReadReg(uint8_t Reg)
{
    uint8_t data = 0;
    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &Reg,  1, 100);
    HAL_SPI_Receive (NRF24_SPI, &data, 1, 100);
    CS_UnSelect();
    return data;
}

void nrf24_ReadReg_Multi(uint8_t Reg, uint8_t *data, int size)
{
    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &Reg, 1,    100);
    HAL_SPI_Receive (NRF24_SPI, data, size, 1000);
    CS_UnSelect();
}

void nrfsendCmd(uint8_t cmd)
{
    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
    CS_UnSelect();
}

/* ------------------------------------------------------------------ */
/*  nrf24_reset                                                         */
/* ------------------------------------------------------------------ */
void nrf24_reset(uint8_t REG)
{
    if (REG == STATUS)
    {
        nrf24_WriteReg(STATUS, 0x70);
    }
    else if (REG == FIFO_STATUS)
    {
        nrf24_WriteReg(FIFO_STATUS, 0x11);
    }
    else
    {
        nrf24_WriteReg(CONFIG,     0x08);
        nrf24_WriteReg(EN_AA,      0x3F);
        nrf24_WriteReg(EN_RXADDR,  0x03);
        nrf24_WriteReg(SETUP_AW,   0x03);
        nrf24_WriteReg(SETUP_RETR, 0x03);
        nrf24_WriteReg(RF_CH,      0x02);
        nrf24_WriteReg(RF_SETUP,   0x0E);
        nrf24_WriteReg(STATUS,     0x70);

        uint8_t rx_p0[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
        nrf24_WriteRegMulti(RX_ADDR_P0, rx_p0, 5);
        uint8_t rx_p1[5] = {0xC2,0xC2,0xC2,0xC2,0xC2};
        nrf24_WriteRegMulti(RX_ADDR_P1, rx_p1, 5);
        nrf24_WriteReg(RX_ADDR_P2, 0xC3);
        nrf24_WriteReg(RX_ADDR_P3, 0xC4);
        nrf24_WriteReg(RX_ADDR_P4, 0xC5);
        nrf24_WriteReg(RX_ADDR_P5, 0xC6);
        uint8_t tx_def[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
        nrf24_WriteRegMulti(TX_ADDR, tx_def, 5);
        nrf24_WriteReg(RX_PW_P0,  0);
        nrf24_WriteReg(RX_PW_P1,  0);
        nrf24_WriteReg(RX_PW_P2,  0);
        nrf24_WriteReg(RX_PW_P3,  0);
        nrf24_WriteReg(RX_PW_P4,  0);
        nrf24_WriteReg(RX_PW_P5,  0);
        nrf24_WriteReg(FIFO_STATUS, 0x11);
        nrf24_WriteReg(DYNPD,  0);
        nrf24_WriteReg(FEATURE, 0);
    }
}

/* ------------------------------------------------------------------ */
/*  NRF24_Init                                                          */
/*  FIX: Không ghi 0 vào CONFIG (gây power-down chip).                 */
/* ------------------------------------------------------------------ */
void NRF24_Init(void)
{
    CE_Disable();
    nrf24_reset(0);
    nrfsendCmd(FLUSH_TX);
    nrfsendCmd(FLUSH_RX);
    nrf24_WriteReg(STATUS, 0x70);
    /* Chip đang power-down. TxMode/RxMode sẽ set PWR_UP=1. */
}

/* ------------------------------------------------------------------ */
/*  NRF24_TxMode                                                        */
/*  FIX: Không gọi CE_Enable(). CE được pulse trong Transmit.          */
/* ------------------------------------------------------------------ */
void NRF24_TxMode(uint8_t *Address, uint8_t channel)
{
    CE_Disable();

    nrf24_WriteReg(RF_CH, channel);
    nrf24_WriteRegMulti(TX_ADDR, Address, 5);

    nrf24_WriteReg(EN_AA,      0x00);   /* tắt Auto-ACK */
    nrf24_WriteReg(SETUP_RETR, 0x00);   /* tắt Auto-Retransmit */

    uint8_t config = nrf24_ReadReg(CONFIG);
    config |=  (1 << 1);   /* PWR_UP = 1  */
    config &= ~(1 << 0);   /* PRIM_RX = 0 */
    nrf24_WriteReg(CONFIG, config);

    HAL_Delay(2);
}

/* ------------------------------------------------------------------ */
/*  NRF24_Transmit                                                      */
/*  FIX: Poll TX_DS thay vì delay cố định. Flush nếu thất bại.        */
/*  Return: 1 = thành công, 0 = thất bại                               */
/* ------------------------------------------------------------------ */
uint8_t NRF24_Transmit(uint8_t *data)
{
    uint8_t cmd;
    uint8_t status;
    uint32_t t_start;

    /* Clear flag cũ */
    nrf24_WriteReg(STATUS, 0x70);

    /* Nạp 32-byte payload vào TX FIFO */
    CS_Select();
    cmd = W_TX_PAYLOAD;
    HAL_SPI_Transmit(NRF24_SPI, &cmd,  1,  100);
    HAL_SPI_Transmit(NRF24_SPI, data, 8, 1000);
    CS_UnSelect();

    /* Pulse CE >= 10 µs */
    CE_Enable();
    HAL_Delay(1);
    CE_Disable();

    /* Poll STATUS: chờ TX_DS (bit 5) hoặc MAX_RT (bit 4), timeout 15 ms */
    t_start = HAL_GetTick();
    do {
        HAL_Delay(1);
        status = nrf24_ReadReg(STATUS);
        if ((HAL_GetTick() - t_start) > 15) break;
    } while (!(status & 0x30));

    if (status & (1 << 5))   /* TX_DS = 1: thành công */
    {
        nrf24_WriteReg(STATUS, 0x70);
        nrfsendCmd(FLUSH_TX);
        return 1;
    }

    /* Thất bại: flush TX để không kẹt gói kế */
    nrfsendCmd(FLUSH_TX);
    nrf24_WriteReg(STATUS, 0x70);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  NRF24_RxMode                                                        */
/*  FIX: Flush RX trước khi bật. EN_AA=0 khớp TX side.                */
/* ------------------------------------------------------------------ */
void NRF24_RxMode(uint8_t *Address, uint8_t channel)
{
    CE_Disable();

    nrf24_WriteReg(STATUS, 0x70);
    nrfsendCmd(FLUSH_RX);

    nrf24_WriteReg(RF_CH, channel);

    nrf24_WriteReg(EN_AA,      0x00);   /* tắt Auto-ACK – khớp TX */
    nrf24_WriteReg(SETUP_RETR, 0x00);

    uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
    en_rxaddr |= (1 << 1);             /* enable Pipe 1 */
    nrf24_WriteReg(EN_RXADDR, en_rxaddr);

    nrf24_WriteRegMulti(RX_ADDR_P1, Address, 5);
    nrf24_WriteReg(RX_PW_P1, 8);      /* fixed 8-byte payload */

    uint8_t config = nrf24_ReadReg(CONFIG);
    config |= (1 << 1) | (1 << 0);    /* PWR_UP=1, PRIM_RX=1 */
    nrf24_WriteReg(CONFIG, config);

    HAL_Delay(2);

    CE_Enable();   /* CE HIGH liên tục = lắng nghe */
}

/* ------------------------------------------------------------------ */
/*  isDataAvailable                                                     */
/*  FIX: Đọc RX_P_NO đúng: bit [3:1] của STATUS.                      */
/* ------------------------------------------------------------------ */
uint8_t isDataAvailable(int pipenum)
{
    uint8_t status = nrf24_ReadReg(STATUS);

    if (status == 0xFF) return 0;   /* SPI lỗi */

    if (status & (1 << 6))          /* RX_DR = bit 6 */
    {
        uint8_t pipe = (status >> 1) & 0x07;
        if (pipe == (uint8_t)pipenum)
        {
            nrf24_WriteReg(STATUS, (1 << 6));   /* clear RX_DR */
            return 1;
        }
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/*  NRF24_Receive                                                       */
/* ------------------------------------------------------------------ */
void NRF24_Receive(uint8_t *data)
{
    uint8_t cmd = R_RX_PAYLOAD;
    CS_Select();
    HAL_SPI_Transmit(NRF24_SPI, &cmd,  1,  100);
    HAL_SPI_Receive (NRF24_SPI, data, 8, 1000);
    CS_UnSelect();
    HAL_Delay(1);
    nrfsendCmd(FLUSH_RX);
}

/* ------------------------------------------------------------------ */
/*  NRF24_ReadAll  (debug)                                              */
/* ------------------------------------------------------------------ */
void NRF24_ReadAll(uint8_t *data)
{
    for (int i = 0; i < 10; i++)
        *(data + i) = nrf24_ReadReg(i);

    nrf24_ReadReg_Multi(RX_ADDR_P0, data + 10, 5);
    nrf24_ReadReg_Multi(RX_ADDR_P1, data + 15, 5);

    *(data + 20) = nrf24_ReadReg(RX_ADDR_P2);
    *(data + 21) = nrf24_ReadReg(RX_ADDR_P3);
    *(data + 22) = nrf24_ReadReg(RX_ADDR_P4);
    *(data + 23) = nrf24_ReadReg(RX_ADDR_P5);

    nrf24_ReadReg_Multi(TX_ADDR, data + 24, 5);

    for (int i = 29; i < 38; i++)
        *(data + i) = nrf24_ReadReg(i - 12);
}
