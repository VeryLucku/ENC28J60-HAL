#include "enc28j60.h"

uint8_t Enc28j60Bank;
uint8_t macaddr[6] = MAC_ADDR;
int gNextPacketPtr;

struct
{
    uint16_t nextPacket;
    uint16_t byteCount;
    uint16_t status;
} header;


extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi1;

static void Error(void)
{
}

static uint8_t SPIx_WriteRead(uint8_t Byte)
{
    uint8_t receivedbyte = 0;
    if (HAL_SPI_TransmitReceive(&hspi1, (uint8_t *)&Byte, (uint8_t *)&receivedbyte, 1, 0x1000) != HAL_OK)
    {
        Error();
    }
    return receivedbyte;
}

void SPI_SendByte(uint8_t bt)
{
    SPIx_WriteRead(bt);
}

uint8_t SPI_ReceiveByte(void)
{
    uint8_t bt = SPIx_WriteRead(0xFF);
    return bt; // вернём регистр данных
}

void enc28j60_writeOp(uint8_t op, uint8_t address, uint8_t data)
{
    SS_SELECT();
    SPI_SendByte(op | (address & ADDR_MASK));
    SPI_SendByte(data);
    SS_DESELECT();
}

static uint8_t enc28j60_readOp(uint8_t op, uint8_t address)
{
    uint8_t result;
    SS_SELECT();
    SPI_SendByte(op | (address & ADDR_MASK));
    SPI_SendByte(0x00);
    // skip dummy byte
    if (address & 0x80)
        SPI_ReceiveByte();
    result = SPI_ReceiveByte();
    SS_DESELECT();
    return result;
}

static void enc28j60_readBuf(uint16_t len, uint8_t *data)
{
    SS_SELECT();
    SPI_SendByte(ENC28J60_READ_BUF_MEM);
    while (len--)
    {
        *data++ = SPIx_WriteRead(0x00);
    }
    SS_DESELECT();
}

static void enc28j60_SetBank(uint8_t address)
{
    if ((address & BANK_MASK) != Enc28j60Bank)
    {
        enc28j60_writeOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_BSEL1 | ECON1_BSEL0);
        Enc28j60Bank = address & BANK_MASK;
        enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, Enc28j60Bank >> 5);
    }
}

static void enc28j60_writeRegByte(uint8_t address, uint8_t data)
{
    enc28j60_SetBank(address);
    enc28j60_writeOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

static uint8_t enc28j60_readRegByte(uint8_t address)
{
    enc28j60_SetBank(address);
    return enc28j60_readOp(ENC28J60_READ_CTRL_REG, address);
}

static void enc28j60_writeReg(uint8_t address, uint16_t data)
{
    enc28j60_writeRegByte(address, data);
    enc28j60_writeRegByte(address + 1, data >> 8);
}

static void enc28j60_writePhy(uint8_t addres, uint16_t data)

{

    enc28j60_writeRegByte(MIREGADR, addres);

    enc28j60_writeReg(MIWR, data);

    while (enc28j60_readRegByte(MISTAT) & MISTAT_BUSY)

        ;
}

void enc28j60_ini(void)
{
    enc28j60_writeReg(ERXST, RXSTART_INIT);
    enc28j60_writeReg(ERXRDPT, RXSTART_INIT);
    enc28j60_writeReg(ERXND, RXSTOP_INIT);
    enc28j60_writeReg(ETXST, TXSTART_INIT);
    enc28j60_writeReg(ETXND, TXSTOP_INIT);

    // Enable Broadcast
    enc28j60_writeRegByte(ERXFCON, enc28j60_readRegByte(ERXFCON) | ERXFCON_BCEN);

    // setup channel level
    // enable packet recieving (MARXEN) and hardware flow control (TXPAUS, RXPAUS)
    enc28j60_writeRegByte(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
    enc28j60_writeRegByte(MACON2, 0x00);
    // enable padding (PADCFG) (alignment packet to 60 bytes and add contol summ (4 bytes))
    // enable length of packet control (FRMLNEN)
    // auto add control sum (TXCRCEN)
    enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);
    // standart values to set full duplex delays
    enc28j60_writeReg(MAIPG, 0x0C12);
    // delay between frames
    enc28j60_writeRegByte(MABBIPG, 0x12);
    // max frame size
    enc28j60_writeReg(MAMXFL, MAX_FRAMELEN);
    // set MAC addres
    enc28j60_writeRegByte(MAADR5, macaddr[0]);
    enc28j60_writeRegByte(MAADR4, macaddr[1]);
    enc28j60_writeRegByte(MAADR3, macaddr[2]);
    enc28j60_writeRegByte(MAADR2, macaddr[3]);
    enc28j60_writeRegByte(MAADR1, macaddr[4]);
    enc28j60_writeRegByte(MAADR0, macaddr[5]);

    enc28j60_writeRegByte(MAADR0, macaddr[5]);

    // setup physical level
    // disable loopback
    enc28j60_writePhy(PHCON2, PHCON2_HDLDIS);
    // leds
    enc28j60_writePhy(PHLCON, PHLCON_LACFG2 |

                                  PHLCON_LBCFG2 | PHLCON_LBCFG1 | PHLCON_LBCFG0 |

                                  PHLCON_LFRQ0 | PHLCON_STRCH);

    enc28j60_SetBank(ECON1);
    // enable global interrupts
    enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);
    // enable recieving of packets
    enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
}

uint16_t enc28j60_packetReceive(uint8_t *buf, uint16_t buflen)
{
    uint16_t len = 0;

    if (enc28j60_readRegByte(EPKTCNT) > 0)
    {
        enc28j60_writeReg(ERDPT, gNextPacketPtr);
        enc28j60_writeReg(ERDPT, gNextPacketPtr);
        enc28j60_readBuf(sizeof header, (uint8_t *)&header);
        gNextPacketPtr = header.nextPacket;
        gNextPacketPtr = header.nextPacket;

        len = header.byteCount - 4; // remove the CRC count
        if (len > buflen)
            len = buflen;

        if ((header.status & 0x80) == 0)
            len = 0;
        else
            enc28j60_readBuf(len, buf);
        buf[len] = 0;

        if (gNextPacketPtr - 1 > RXSTOP_INIT)
            enc28j60_writeReg(ERXRDPT, RXSTOP_INIT);
        else
            enc28j60_writeReg(ERXRDPT, gNextPacketPtr - 1);

        enc28j60_writeReg(ERXRDPT, gNextPacketPtr - 1);

        enc28j60_writeOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    }

    return len;
}
