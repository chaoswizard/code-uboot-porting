/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002, 2010
 * David Mueller, ELSOFT AG, <d.mueller@elsoft.ch>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <netdev.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>

DECLARE_GLOBAL_DATA_PTR;

#define FCLK_SPEED 2   //wx:replace 1

#if FCLK_SPEED==0		/* Fout = 203MHz, Fin = 12MHz for Audio */
#define M_MDIV	0xC3
#define M_PDIV	0x4
#define M_SDIV	0x1
#elif FCLK_SPEED==1		/* Fout = 202.8MHz */
#define M_MDIV	0xA1
#define M_PDIV	0x3
#define M_SDIV	0x1
#elif FCLK_SPEED==2    /*wx:add: Fout=405MHz*/
#define M_MDIV	0x7F  // ref s3c2440 spec:PLL VALUE SELECTION
#define M_PDIV	0x2
#define M_SDIV	0x1
#endif

#define USB_CLOCK 2  //wx:replace 1

#if USB_CLOCK==0 
#define U_M_MDIV	0xA1
#define U_M_PDIV	0x3
#define U_M_SDIV	0x1
#elif USB_CLOCK==1
#define U_M_MDIV	0x48
#define U_M_PDIV	0x3
#define U_M_SDIV	0x2
#elif USB_CLOCK==2       // Fout = 48Mhz
#define U_M_MDIV	0x38
#define U_M_PDIV	0x2
#define U_M_SDIV	0x2
#endif

static inline void pll_delay(unsigned long loops)
{
	__asm__ volatile ("1:\n"
	  "subs %0, %1, #1\n"
	  "bne 1b":"=r" (loops):"0" (loops));
}

/*
 * Miscellaneous platform dependent initialisations
 */

int board_early_init_f(void)
{
	struct s3c24x0_clock_power * const clk_power =
					s3c24x0_get_base_clock_power();
	struct s3c24x0_gpio * const gpio = s3c24x0_get_base_gpio();

	/* to reduce PLL lock time, adjust the LOCKTIME register */
	writel(0xFFFFFF, &clk_power->locktime);

	/* configure MPLL */
	writel((M_MDIV << 12) + (M_PDIV << 4) + M_SDIV,
	       &clk_power->mpllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(4000);

	/* configure UPLL */
	writel((U_M_MDIV << 12) + (U_M_PDIV << 4) + U_M_SDIV,
	       &clk_power->upllcon);

	/* some delay between MPLL and UPLL */
	pll_delay(8000);

	/* set up the I/O ports */
	writel(0x007FFFFF, &gpio->gpacon);
   /* wx: GPB10 and GPB9 Config as External DMA CON, lookup CON5 PIN13,14
    * of Mini2440 shema, GPB0 output 0(Speaker),GPB1 input 1, other output 1)
    */
	writel(0x00295551, &gpio->gpbcon);
	writel(0x000007FE, &gpio->gpbup);

	/* wx: GPC5 set as gpio output , other for lcd control,
	 * GPC5 output as 0(usb disable, connected to usb D+)
	 */
	writel(0xAAAAA6AA, &gpio->gpccon);
	writel(readl(&gpio->gpcdat)&(~(1<<5)),&gpio->gpcdat);
	writel(0x0000FFFF, &gpio->gpcup);
	
	writel(0xAAAAAAAA, &gpio->gpdcon);
	writel(0x0000FFFF, &gpio->gpdup);
	writel(0xAAAAAAAA, &gpio->gpecon);
	writel(0x0000FFFF, &gpio->gpeup);
	writel(0x000055AA, &gpio->gpfcon);
	writel(0x000000FF, &gpio->gpfup);
	writel(0xFF95FFBA, &gpio->gpgcon);
	writel(0x0000FFFF, &gpio->gpgup);
	writel(0x002AFAAA, &gpio->gphcon);
	writel(0x000007FF, &gpio->gphup);

	return 0;
}

int board_init(void)
{
	/* arch number of mini2440-Board*/
	gd->bd->bi_arch_number = MACH_TYPE_MINI2440;//MACH_TYPE_SMDK2410
	

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x30000100;

	icache_enable();
	dcache_enable();

	return 0;
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	//wx: this will be called at initquence table (board.c:init_sequence last item)
	gd->ram_size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_CMD_NET
int board_eth_init(bd_t *bis)
{
	int rc = 0;
    
#ifdef CONFIG_CS8900
    cs8900_initialize(0, CONFIG_CS8900_BASE);
#endif
    
#ifdef CONFIG_DRIVER_DM9000
	rc = dm9000_initialize(bis);
#endif
	return rc;
}
#endif

/*
 * Hardcoded flash setup:
 * Flash 0 is a non-CFI AMD AM29LV800BB flash.
 */
ulong board_flash_get_legacy(ulong base, int banknum, flash_info_t *info)
{
	/*wx:8 x 2(portwidth) (S29AL160J BYTE#(PIN 47) is 1, hense, 
	   we select word(16bit) mode)*/
	info->portwidth = FLASH_CFI_16BIT; 
	/*wx:8 x 2 = 16(chipwidth)(mini2440,OM[1] PIN is 0, OM[0] PIN is 1, hense
	   we select 16bit mode(bank0 data width mode),*/
	info->chipwidth = FLASH_CFI_BY16;  
	info->interface = FLASH_CFI_X16;
	return 1;
}
