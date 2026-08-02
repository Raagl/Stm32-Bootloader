
#include "bootloader_uart.h"
#include <string.h>

static error_t bootloader_uart_read_data(void);
static void bootloader_parse_command(uint8_t *buffer);
static void bootloader_send_ack(uint8_t len);
static void bootloader_send_nack(void);
static uint8_t verify_address(uint32_t addr);
static void bootloader_handle_get_ver(void);
static void bootloader_handle_get_help(void);
static void bootloader_handle_get_cid(void);
static void bootloader_handle_get_rdp_status(void);
static void bootloader_handle_go_to_addr(void);
static void bootloader_handle_flash_erase(void);
static void bootloader_handle_write_mem(void);
static void bootloader_handle_en_r_w_protect(void);
static void bootloader_handle_read_sector_status(void);
static void bootloader_handle_dis_r_w_protect(void);

uint8_t rx_buffer[BL_RX_BUFFER_SIZE];
uint32_t received_crc;

uint8_t supported_commands[] = { BL_GET_VER, BL_GET_HELP, BL_GET_CID, BL_GET_RDP_STATUS,
		BL_GO_TO_ADDR,BL_FLASH_ERASE, BL_MEM_WRITE,
		BL_EN_R_W_PROTECT, BL_READ_SECTOR_P_STATUS, BL_DIS_R_W_PROTECT};

void bootloader_uart_mode(void)
{
	led_on();
    while (1)
    {
    	if(bootloader_uart_read_data() == ERR_OK)
    	{
    		bootloader_parse_command(rx_buffer);
    	}
    }
}

static error_t bootloader_uart_read_data(void)
{
    uint8_t rcv_len = 0;
    error_t status = ERR_OK;

    memset(rx_buffer, 0, BL_RX_BUFFER_SIZE);

    /* Receive first byte (length) */
    uart_receive(rx_buffer, 1);

    rcv_len = rx_buffer[0];

    /* Validate length */
    if ((rcv_len < 5) || (rcv_len > (BL_RX_BUFFER_SIZE - 1)))
    {
        bootloader_send_nack();
        status = ERR_FAIL;
    }

    /* Receive remaining bytes */
    uart_receive(&rx_buffer[1], rcv_len);



    return status;
}

static void bootloader_parse_command(uint8_t *buffer)
{
    uint8_t cmd = buffer[1];

    switch(cmd)
    {
        case BL_GET_VER:
            bootloader_handle_get_ver();
            break;

        case BL_GET_HELP:
        	bootloader_handle_get_help();
        	break;

        case BL_GET_CID:
        	bootloader_handle_get_cid();
        	break;

        case BL_GET_RDP_STATUS:
        	bootloader_handle_get_rdp_status();
        	break;

        case BL_GO_TO_ADDR:
        	bootloader_handle_go_to_addr();
        	break;

        case BL_FLASH_ERASE:
        	bootloader_handle_flash_erase();
            break;

        case BL_MEM_WRITE:
        	bootloader_handle_write_mem();
            break;

        case BL_EN_R_W_PROTECT:
        	bootloader_handle_en_r_w_protect();
        	break;

        case BL_READ_SECTOR_P_STATUS:
        	bootloader_handle_read_sector_status();
        	break;

        case BL_DIS_R_W_PROTECT:
        	bootloader_handle_dis_r_w_protect();
        	break;

        default:
            bootloader_send_nack();
            break;
    }
}

static void bootloader_handle_get_ver(void)
{
    uint8_t version = BL_VERSION;

    uint8_t rcv_len = rx_buffer[0];

    /*Seperate the received CRC*/
    memcpy(&received_crc,&rx_buffer[rcv_len-3],sizeof(received_crc));

    if((crc_verify(rx_buffer, rcv_len - 4, received_crc)) ==ERR_OK)
	{
	  bootloader_send_ack(1);
	  uart_transmit(&version,1);
	}
	else
	{
	   bootloader_send_nack();
	}
}


static void bootloader_handle_get_help(void)
{
	bootloader_send_ack(sizeof(supported_commands));
	uart_transmit(supported_commands, sizeof(supported_commands));
}

static void bootloader_handle_get_cid(void)
{
	uint16_t CID;
	CID = (uint16_t)(DBGMCU->IDCODE) & 0x0FFF;
	bootloader_send_ack(2);
	uart_transmit((uint8_t *)&CID, 2);
}

static void bootloader_handle_get_rdp_status(void)
{
	uint8_t rdp;
	rdp = (uint8_t)(FLASH->OPTCR & FLASH_OPTCR_RDP);
	bootloader_send_ack(1);
	uart_transmit(&rdp, 1);
}

static void bootloader_handle_go_to_addr(void)
{
	uint32_t addr;
	uint8_t addr_valid = ADDR_VALID;
	uint8_t addr_invalid = ADDR_INVALID;

	bootloader_send_ack(1);

	memcpy(&addr,
	       &rx_buffer[2],
	       sizeof(addr));

	if (verify_address(addr) == ADDR_VALID)
	{
		uart_transmit(&addr_valid, 1);
		jump_to_app(addr);
	}
	else
	{
	    uart_transmit(&addr_invalid,1);
	}
}

static void bootloader_handle_flash_erase(void)
{
	uint8_t sector;
	uint8_t nsectors;
	uint8_t status;

	sector = (uint8_t)rx_buffer[2];
	nsectors = (uint8_t)rx_buffer[3];

	status = flash_erase(sector, nsectors);

	bootloader_send_ack(1);

	uart_transmit(&status, 1);

}

static void bootloader_handle_write_mem(void)
{
	/*Packet Format:
	 *
	 *+--------------------------------------------------------------+
		| LEN | CMD | ADDRESS(4) | PAYLOAD_LEN | DATA | CRC32 (4 bytes)|
	  +--------------------------------------------------------------+
	 *
	 * */
	uint32_t mem_address;
	uint8_t payload_length;
	uint8_t* data;
	uint8_t status;

	/*Extract the write memory address*/
	memcpy(&mem_address, &rx_buffer[2], sizeof(mem_address));

	/*Extract the write payload length*/
	payload_length = rx_buffer[6];

	/*Get the data/bin file*/
	data = &rx_buffer[7];

	bootloader_send_ack(1);

	if (verify_address(mem_address) == ADDR_VALID)
	{
		status = flash_program(mem_address, payload_length, data);
	}
	else
	{
		status = ADDR_INVALID;
	}

	uart_transmit(&status,1);
}

static void bootloader_handle_en_r_w_protect(void)
{
	uint8_t sector_mask;
	uint8_t protection_mode;
	uint8_t status;

	sector_mask = rx_buffer[2];
	protection_mode = rx_buffer[3];

	bootloader_send_ack(1);

	status = flash_enable_rw_protection(sector_mask, protection_mode);

	uart_transmit(&status, 1);
}


static void bootloader_handle_read_sector_status(void)
{

	uint8_t status;

	status = flash_get_sector_status();

	bootloader_send_ack(2);

	uart_transmit(&status, 2);

}

static void bootloader_handle_dis_r_w_protect(void)
{
	uint8_t status;

	status = flash_disable_rw_protection();

	bootloader_send_ack(2);

	uart_transmit(&status, 1);

}



static void bootloader_send_ack(uint8_t len)
{
    uint8_t ack[2];

    ack[0] = BL_ACK;
    ack[1] = len;

    uart_transmit(ack,2);
}

static void bootloader_send_nack(void)
{
    uint8_t nack = BL_NACK;

    uart_transmit(&nack,1);
}

static uint8_t verify_address(uint32_t addr)
{
    if ((addr >= FLASH_BASE) && (addr <= FLASH_END))
    {
        return ADDR_VALID;
    }

    if ((addr >= SRAM1_BASE) && (addr <= SRAM1_END))
    {
        return ADDR_VALID;
    }

    return ADDR_INVALID;
}
