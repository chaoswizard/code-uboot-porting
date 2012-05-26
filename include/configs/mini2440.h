/*
 * (C) Copyright 2012
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <garyj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Modified for the friendly-arm mini2440 by
 * wang_xi@anyka.com
 * (C) Copyright 2012
 *
 * Configuation settings for the friendly-arm mini2440 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM920T		/* This is an ARM920T Core */
#define CONFIG_S3C24X0		/* in a SAMSUNG S3C24x0-type SoC */
#define CONFIG_S3C2440		/* specifically a SAMSUNG S3C2440 SoC */
#define CONFIG_MINI2440		


//wx: if nand boot img size is larger than 4K, need open this to reload
#define CONFIG_NAND_BOOT //CONFIG_NAND_SPL  


#ifdef CONFIG_NAND_BOOT
//#define CONFIG_SKIP_LOWLEVEL_INIT 
/*wx: 0x33000000->0x34000000 (48M:16M)*/
#define CONFIG_SYS_TEXT_BASE           0x33000000 
#else
/*wx:if run on ram, must open it*/
#define CONFIG_SKIP_LOWLEVEL_INIT  
/*wx: 0x33400000->0x34000000 (52M:12M)*/
#define CONFIG_SYS_TEXT_BASE           0x33400000
#endif

#define CONFIG_SYS_ARM_CACHE_WRITETHROUGH

/* input clock of PLL (the mini2440 has 12MHz input clock) */
#define CONFIG_SYS_CLK_FREQ	12000000

#undef CONFIG_USE_IRQ		/* we don't need IRQ/FIQ stuff */

#define CONFIG_CMDLINE_TAG	/* enable passing of ATAGs */
/*'wx: if defined this macro, uboot will auto create the memery info tag, if not
 * MUST describle in CONFIG_BOOTARGS "mem=xx"(e.g. mem=64M)
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG

/*
 * Hardware drivers
 */
#if 1
/*mini2440 use DM9000EP which conected with nGCS4(0x20000000)*/
#define CONFIG_DRIVER_DM9000
#define CONFIG_DM9000_NO_SROM
#define CONFIG_DM9000_BASE       0x20000300
#define DM9000_IO                CONFIG_DM9000_BASE
#define DM9000_DATA             (CONFIG_DM9000_BASE + 4)
//#define CONFIG_DM9000_DEBUG
#else
#define CONFIG_CS8900		/* we have a CS8900 on-board */
#define CONFIG_CS8900_BASE	0x19000300
#define CONFIG_CS8900_BUS16	/* the Linux driver does accesses as shorts */
#endif

/*
 * select serial console configuration
 */
#define CONFIG_S3C24X0_SERIAL
#define CONFIG_SERIAL1		1	/* we use SERIAL 1 on mini2440 */

/************************************************************
 * USB support (currently only works with D-cache off)
 ************************************************************/
#define CONFIG_USB_OHCI
#define CONFIG_USB_KEYBOARD
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION

/************************************************************
 * RTC
 ************************************************************/
#define CONFIG_RTC_S3C24X0


#define CONFIG_BAUDRATE		115200

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME

/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_BSP
#define CONFIG_CMD_CACHE
#define CONFIG_CMD_DATE
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ELF
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_REGINFO
#define CONFIG_CMD_USB
#define CONFIG_CMD_NET

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#define CONFIG_CMDLINE_EDITING

/* autoboot */
#define CONFIG_BOOTDELAY	9
#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_RESET_TO_RETRY
#define CONFIG_ZERO_BOOTDELAY_CHECK


// nand flash boot, rootfs at mtd3, refer to kernel partition.
#define CONFIG_BOOTARGS         	\
	"noinitrd "\
	"console=ttySAC0,115200 "\
	"root=/dev/mtdblock2 rw "\
	"rootfstype=yaffs "\
    "init=/linuxrc"




//wx: the Head 3 bytes MUST is a valid Value,or else the MAC will set Failed.
#define CONFIG_ETHADDR      12:34:56:85:09:09
#define CONFIG_NETMASK		255.255.255.0
//wx: because my wireless LAN has used 192.168.0.X net, and I seted wired LAN on 192.168.9.X net
#define CONFIG_IPADDR		192.168.9.1
#define CONFIG_SERVERIP 	192.168.9.2
#define CONFIG_GATEWAYIP    192.168.9.2

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	115200	/* speed to run kgdb serial port */
/* what's this ? it's not used anywhere */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

/*
 * Miscellaneous configurable options
 */
#define CONFIG_SYS_LONGHELP		/* undef to save memory */
#define CONFIG_SYS_PROMPT	"[WangXi@MINI2440]#" //wx:comment:boot command line prefix string
#define CONFIG_SYS_CBSIZE	256
/* Print Buffer Size */
#define CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE + \
				sizeof(CONFIG_SYS_PROMPT)+16)
#define CONFIG_SYS_MAXARGS	16
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

/* may be activated as soon as s3c24x0 has print_cpuinfo support */
/*#define CONFIG_DISPLAY_CPUINFO*/		/* Display cpu info */

#define CONFIG_SYS_MEMTEST_START	0x30000000	/* memtest works on */
#define CONFIG_SYS_MEMTEST_END		0x33F00000	/* 63 MB in DRAM */

#define CONFIG_SYS_LOAD_ADDR		0x30008000

#define CONFIG_SYS_HZ			1000

/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/* support additional compression methods */
#define CONFIG_BZIP2
#define CONFIG_LZO
#define CONFIG_LZMA

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1          /* we have 1 bank of DRAM */
//wx:Sdram HY57V561620FTP/K4S561632N is conected with nGCS6(0x30000000),and maped to its first bank.
#define PHYS_SDRAM_1		0x30000000 /* SDRAM Bank #0 */
#define PHYS_SDRAM_1_SIZE	0x04000000 /* wx:32M X 2=64M */

//wx:Norfalsh SST39VF1601/S29AL016 is conected with NGCS0(0x00000000)
//this will be a member of cfi_flash_bank_addr
#define PHYS_FLASH_1		0x00000000 /* Flash Bank #0 */

#define CONFIG_SYS_FLASH_BASE	PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * NorFLASH and environment organization
 */

/* wx:comment: bleow is the configure of NorFlash S29AL016(Supplier:Spansion)
 *   a.Common Flash Interface(CFI) is an open specification,May be implemently
 * by vendor:Inter,AMD,Sharp,Fujitsu.
 *   b.Legacy Component:JEDEC Stardard, not CFI.
 *   c.Spansion is a supplier which established by AMD and Fujistu, This NorFlash
 *  is CFI compliant and compatility JEDEC Standard. 
 */
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_FLASH_CFI_DRIVER//wx: enable compile cfi_flash.o
//#define CONFIG_SYS_CFI_FLASH_CONFIG_REGS {0xffff} 
#define CONFIG_SYS_FLASH_LEGACY_1024Kx16          //(1M x 16bit = 2M byte)
#define CONFIG_FLASH_SHOW_PROGRESS	45

#define CONFIG_SYS_MAX_FLASH_BANKS	1 
#define CONFIG_SYS_FLASH_BANKS_LIST     { CONFIG_SYS_FLASH_BASE }/*wx:comment this will be a member list,_as the return of_cfi_flash_bank_addr*/

/* If use SST39VF1601, operator mode is block and sector, when use sector mode,
 * and 1 sector=2K word = 4K byte, so the sector is (2M / 4K) = 512
 * If use S29AL016, has 35 sectors (spec:P20),and 64Kbytes per sector.
 */
#define CONFIG_SYS_MAX_FLASH_SECT	(35) //wx:replace:(19)


#if defined (CONFIG_NAND_BOOT)
#define CONFIG_ENV_IS_IN_NAND //CONFIG_ENV_IS_IN_FLASH//
#else
#define CONFIG_ENV_IS_IN_FLASH
#endif


/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE



/*
 * Size of malloc() pool
 * BZIP2 / LZO / LZMA need a lot of RAM
 */
#define CONFIG_SYS_MALLOC_LEN	(4 * 1024 * 1024)

#define CONFIG_SYS_MONITOR_LEN	(448 * 1024)
#define CONFIG_SYS_MONITOR_BASE	CONFIG_SYS_FLASH_BASE

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_S3C2440

#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS			1 //wx: all nand chips count
/*wx:s3c2400 NandFlash controler register base address,for r/w K9F2G08U0B*/
#define CONFIG_SYS_NAND_BASE		0x4E000000 

#define CONFIG_S3C2440_NAND_HWECC
#define CONFIG_SYS_NAND_ECCSIZE		2048
#define	CONFIG_SYS_NAND_ECCBYTES	4
#define CONFIG_MTD_NAND_VERIFY_WRITE    1
#endif

/*
 * File system
 */
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
//wx: write nand as yaffs
#define CONFIG_CMD_NAND_YAFFS

#define CONFIG_YAFFS2
#define CONFIG_RBTREE


#define BT_XMK_STR(x)	#x
#define BT_MK_STR(x)	BT_XMK_STR(x)


#define  IMG_UBOOT_PATH      "u-boot.bin"
#define  IMG_KERNEL_PATH     "uImage"
#define  IMG_ROOTFS_PATH     "rootfs.img"

/**
*  NAND Partion Will Create as bellow layout when burn img to nand
*  0  - 4M : boot (here we only use 0-1M, 1M-4M unused)[burn as raw]
*  4M - 8M : kernel(here we only use 4-7M, 7M-8M unused)[burn as raw]
*  8M - ?  : fs [burn as fs]
*
*!NOTE:
* [burn as raw] is for u-boot read as raw mode when load the kernel image(u-boot 
*  not support read nand as yaffs mode[default code])
* [burn as fs] is for kernel mount nand as fs mode when kernel init.(MUST make
*  image to fs, and read as fs mode for load image)
* So FrinedlyArm-linux-kernnel nand partition as bellow:
* (refer to the linux-kernel: mach-mini2440.c  friendly_arm_default_nand_part[] line282)
* 
static struct mtd_partition friendly_arm_default_nand_part[] = {
	[0] = {
		.name	= "uboot+params",
		.size	= SZ_4M,
		.offset	= 0,
	},
	[1] = {
		.name	= "kernel",
		.offset = SZ_4M,
		.size	= SZ_4M,
	},
	[2] = {
		.name	= "rootfs",
		.offset = SZ_8M,
		.size	= 1024 * 1024 * 1024, //
	},
	[3] = {
		.name	= "whole_nand",
		.offset = 0x00000000,
		.size	= 1024 * 1024 * 1024, //
	}
};

*/

//wx: enable the nand defalut partition manage 
#define MTDIDS_DEFAULT "nand0=nandflash0"
//wx: define the defalut nand partion configure,
/** wx: !NOTE: 
* MUST make sure the offset of root partition same as the target system(linux-kernel defined)
* Here Skip (params) partition, and need not care about the offset because it be
* included in bootloader partition.
*/
#define MTDPARTS_DEFAULT "mtdparts=nandflash0:4M@0(uboot+params)," \
                         "4M(kernel_uImage)," \
                         "-(rootfs)"

//mtdblock0 [0M-1M]/[0M-4M]
#define  IMG_UBOOT_OFFSET     0
#define  IMG_UBOOT_SIZE       0x100000

//mtdblock1 [4M-7M]/[4M-8M]
#define  IMG_KERNEL_OFFSET    0x400000
#define  IMG_KERNEL_SIZE      0x300000

//mtdblock2 [8M-?]
#define  IMG_ROOTFS_OFFSET    0x800000





#if defined(CONFIG_ENV_IS_IN_FLASH)
#define CONFIG_ENV_SIZE			 (0x10000) // 64*1024
#define CONFIG_ENV_ADDR			 (CONFIG_SYS_FLASH_BASE + 0x200000 - CONFIG_ENV_SIZE)//wx:norflash: 2M:0
#elif defined(CONFIG_ENV_IS_IN_NAND)
#define CONFIG_ENV_SIZE			 (0x20000) // 128*1024, wx: MUST mutiply with Block Size(128K), orelse env will write failed
#define CONFIG_ENV_OFFSET      	 (IMG_UBOOT_OFFSET + IMG_UBOOT_SIZE - CONFIG_ENV_SIZE)//wx:nand,2M:254M
#else
#error save environments?
#endif




// boot default 
#define CONFIG_BOOTCOMMAND  \
       "nand read " BT_MK_STR(CONFIG_SYS_LOAD_ADDR) " " BT_MK_STR(IMG_KERNEL_OFFSET) " " BT_MK_STR(IMG_KERNEL_SIZE)\
       ";bootm" " " BT_MK_STR(CONFIG_SYS_LOAD_ADDR)

//-----------------------------------------------------------------------------
//wx: usage: run 'cmdname' on console input line.
#define INSTALL_UBOOT_COMMAND  \
        "tftp " BT_MK_STR(CONFIG_SYS_LOAD_ADDR)  " " IMG_UBOOT_PATH\
        ";nand erase " BT_MK_STR(IMG_UBOOT_OFFSET) " " BT_MK_STR(IMG_UBOOT_SIZE)\
        ";nand write $fileaddr"  " " BT_MK_STR(IMG_UBOOT_OFFSET) " " BT_MK_STR(IMG_UBOOT_SIZE)

#if 0
#define INSTALL_KERNEL_COMMAND  \
        "tftp " BT_MK_STR(CONFIG_SYS_LOAD_ADDR) " " IMG_KERNEL_PATH\
        ";nand erase " BT_MK_STR(IMG_KERNEL_OFFSET) " " BT_MK_STR(IMG_KERNEL_SIZE)\
        ";nand write $fileaddr"  " " BT_MK_STR(IMG_KERNEL_OFFSET) " " BT_MK_STR(IMG_KERNEL_SIZE)
#else
#define INSTALL_KERNEL_COMMAND  \
        "tftp " BT_MK_STR(CONFIG_SYS_LOAD_ADDR) " " IMG_KERNEL_PATH\
        ";nand erase " BT_MK_STR(IMG_KERNEL_OFFSET) "  $filesize"\
        ";nand write $fileaddr"  " " BT_MK_STR(IMG_KERNEL_OFFSET) " $filesize"

#endif
#define INSTALL_ROOTFS_COMMAND  \
        "tftp " BT_MK_STR(CONFIG_SYS_LOAD_ADDR) " "IMG_ROOTFS_PATH\
        ";nand erase " BT_MK_STR(IMG_ROOTFS_OFFSET) "  $filesize"\
        ";nand write.yaffs  $fileaddr " BT_MK_STR(IMG_ROOTFS_OFFSET) " $filesize"


/* additions for new relocation code, must be added to all boards */
//wx:comment physical ram start address.(mini2440 sdram use nGCS6)
#define CONFIG_SYS_SDRAM_BASE	PHYS_SDRAM_1
//  wx:comment: start.s jump to c function use bleow stack address(must init before call c function)
// the GENERATED_GBL_DATA_SIZA(global_data) is create by kbulid tools as in asm-offsets.c
//---------------------------------
//|   (4K)=0x30001000
//|       +sizeof(struct global_data) = gd
//|       =INIT_SP_ADDR | // up is stack, down is system global data
//|       +SP_SIZE ?(dymatic size, decide by gd size)     
//|--------------------------------
//|      boot code
//|------- (nGCS6)=0x30000000  
//---------------------------------  
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + 0x1000 - \
				GENERATED_GBL_DATA_SIZE)

#define CONFIG_BOARD_EARLY_INIT_F

#endif /* __CONFIG_H */
