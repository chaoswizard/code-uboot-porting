/*
 * nand_read.c: Simple NAND read functions for booting from NAND
 *
 * This is used by cpu/arm920/start.S assembler code,
 * and the board-specific linker script must make sure this
 * file is linked within the first 4kB of NAND flash.
 *
 * Taken from GPLv2 licensed vivi bootloader,
 * Copyright (C) 2002 MIZI Research, Inc.
 *
 * Author: Hwang, Chideok <hwang@mizi.com>
 * Date  : $Date: 2004/02/04 10:37:37 $
 *
 * u-boot integration and bad-block skipping (C) 2006 by OpenMoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * sorting code
 * Author: chaoswizard <85855632@qq.com>
 */

#include <common.h>
#include <linux/mtd/nand.h>
#include <asm/io.h>
#include <asm/arch/s3c24x0_cpu.h>


//#define  NAND_BOOT_LOAD_VERIFY
#define  NANDBOOT_IMG_LENGTH   0x80000


#define __READ_B(a)   (*(volatile unsigned char *)(a))
#define __READ_W(a)   (*(volatile unsigned short *)(a))
#define __READ_L(a)   (*(volatile unsigned int *)(a))
#define __WRITE_B(v, a)   (*(volatile unsigned char *)(a) = (v))
#define __WRITE_W(v, a)   (*(volatile unsigned short *)(a) = (v)
#define __WRITE_L(v, a)   (*(volatile unsigned int *)(a) = (v))


#ifdef CONFIG_NAND_BOOT_CMD_TEST
#define NFBT_DEBUG(s, args...)  printf(s, args)
#define NFBT_PUTS(s)            printf(s)
//#define debug(format, ...)   printf(format, __VA_ARGS__) 
//#define debug(format, ...) printf(format, ## __VA_ARGS__) 
#else
#define NFBT_DEBUG(s, args...)  
#define NFBT_PUTS(s)            
#endif
/*--------------- NFCONF --------------------------*/
/*wx: NFCONF[13:12]*/
#define NFCONF_TACLS(x)         ((x) << 12)
/*wx: NFCONF[10:8]*/
#define NFCONF_TWRPH0(x)        ((x) << 8)
/*wx: NFCONF[6:4]*/
#define NFCONF_TWRPH1(x)        ((x) << 4)
/*wx: NFCONF[0]*/
#define NFCONF_BUSWIDTH(x)      ((x) << 0) //HW Set
/*-------------- NFCONT --------------------------*/
/*wx: initialize ecc decoder/encoder*/
#define NFCONT_RnB_MODE         (1 << 8)
#define NFCONT_INITECC_BIT      (1 << 4)
/*wx: NandFlash Chip Enable(low)*/
#define NFCONT_nCE_BIT          (1 << 1)
/*wx: NandFlash Controller Enable(1)/Disable(0)*/
#define NFCONT_MODE_BIT         (1 << 0)
/*--------------- NFSTAT --------------------------*/
/*wx: NFSTAT[2]*/
#define NFSTAT_RnB_BIT         (1 << 2)
/*wx: NFSTAT[1]*/
#define NFSTAT_nCE_BIT         (1 << 1)
/*wx: NFSTAT[0]*/
#define NFSTAT_BUSY_BIT	       (1 << 0)
/*-------------------------------------------------*/
#if defined(CONFIG_NAND_BOOT) || defined(CONFIG_NAND_BOOT_CMD_TEST)
static inline void nand_wait_chip_ready(void)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();
	int i;

	while (!(__READ_B(&nand_reg->nfstat) & NFSTAT_BUSY_BIT)) {
        for (i=0; i<10; i++);
        NFBT_DEBUG("no ready %x: %x\n", 
                   &nand_reg->nfstat,
                   __READ_B(&nand_reg->nfstat));
    }
		
}


static inline void nand_clear_chip_busy(void)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();
	
	__WRITE_B(__READ_B(&nand_reg->nfstat) | NFSTAT_RnB_BIT,
		   &nand_reg->nfstat);	
}

static inline void nand_disable_chip(void)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();

	__WRITE_L(__READ_L(&nand_reg->nfcont) | NFCONT_nCE_BIT, &nand_reg->nfcont);	
}


static inline void nand_enable_chip(void)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();

	__WRITE_L(__READ_L(&nand_reg->nfcont) & ~NFCONT_nCE_BIT, &nand_reg->nfcont);
}


/*--------------------------------------------------------------------*/
struct boot_nand_t {
	int page_size;
#ifdef CONFIG_NAND_BOOT_SKIP_BAD
	int block_size;
	int bad_block_offset;
#endif
};

#ifdef CONFIG_NAND_BOOT_SKIP_BAD
static int is_bad_block(struct boot_nand_t * mtd, unsigned long i)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();

	nand_clear_chip_busy();
	
	if (mtd->page_size == 512) {
	
		__WRITE_B(NAND_CMD_READOOB, /* 0x50 */
		    &nand_reg->nfcmd);
	
		__WRITE_B(mtd->bad_block_offset & 0xf, 
			&nand_reg->nfaddr); /*1st cycle*/
		__WRITE_B((i >> 9) & 0xff, 
			&nand_reg->nfaddr); /*2nd cycle*/
		__WRITE_B((i >> 17) & 0xff, 
			&nand_reg->nfaddr); /*3th cycle*/
		__WRITE_B((i >> 25) & 0xff, 
			&nand_reg->nfaddr); /*4th cycle*/
	
	} else if (mtd->page_size == 2048) {
		unsigned long page_num;
		
		page_num = i >> 11; /* addr / 2048 */

		__WRITE_B(NAND_CMD_READ0, 
		    &nand_reg->nfcmd);
	
		__WRITE_B(mtd->bad_block_offset & 0xff, 
			&nand_reg->nfaddr); /*1st cycle*/
		__WRITE_B((mtd->bad_block_offset >> 8) & 0xff, 
			&nand_reg->nfaddr); /*2nd cycle*/
		__WRITE_B(page_num & 0xff, 
			&nand_reg->nfaddr); /*3th cycle*/
		__WRITE_B((page_num >> 16) & 0xff, 
			&nand_reg->nfaddr); /*4th cycle*/

		__WRITE_B(NAND_CMD_READSTART, 
		    &nand_reg->nfcmd);
		    
	}  else  {
		return -1;
	}
	nand_wait_chip_ready();
	
	if ((__READ_B(&nand_reg->nfdata) & 0xff) != 0xff)
		return 1;

	return 0;
}

#endif

static int nand_read_page_ll(struct boot_nand_t * mtd, unsigned char *buf, unsigned long addr)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();
	unsigned short *ptr16 = (unsigned short *)buf;
	unsigned int i, page_num;

    NFBT_DEBUG("read:%x\n", addr);
   
	nand_clear_chip_busy();
            
	__WRITE_B(NAND_CMD_READ0, &nand_reg->nfcmd);
    NFBT_DEBUG("%x:cmd:%x/%x\n", &nand_reg->nfcmd, __READ_B(&nand_reg->nfcmd), NAND_CMD_READ0);
	
    if (mtd->page_size == 512) {
		/* Write Address */
		__WRITE_B(addr & 0xff, 
			&nand_reg->nfaddr); /*1st cycle*/
		__WRITE_B((addr >> 9) & 0xff, 
			&nand_reg->nfaddr); /*2nd cycle*/
		__WRITE_B((addr >> 17) & 0xff, 
			&nand_reg->nfaddr); /*3th cycle*/
		__WRITE_B((addr >> 25) & 0xff, 
			&nand_reg->nfaddr); /*4th cycle*/

	} else if (mtd->page_size == 2048) {
		page_num = addr >> 11; /* addr / 2048 */
		/* Write Address */
		__WRITE_B(0, 
			&nand_reg->nfaddr); /*1st cycle*/
		__WRITE_B(0, 
			&nand_reg->nfaddr); /*2nd cycle*/
		__WRITE_B(page_num & 0xff, 
			&nand_reg->nfaddr); /*3th cycle*/
		__WRITE_B((page_num >> 8) & 0xff, 
			&nand_reg->nfaddr); /*4th cycle*/
		__WRITE_B((page_num >> 16) & 0xff, 
			&nand_reg->nfaddr); /*5th cycle*/

		__WRITE_B(NAND_CMD_READSTART, 
		    &nand_reg->nfcmd);
	} else {
        NFBT_DEBUG("unsupport page %d\n", mtd->page_size);
		return -1;
	}
	
   	nand_wait_chip_ready();
    
    NFBT_DEBUG("data:-> 0x%x...\n", buf);
    
	for (i = 0; i < (mtd->page_size>>1); i++) {
		*ptr16 = __READ_W(&nand_reg->nfdata);
		ptr16++;
	}
  
	return 0;
}

static unsigned short nand_read_id()
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();
	unsigned short res = 0;

	__WRITE_B(NAND_CMD_READID, &nand_reg->nfcmd);
	
	__WRITE_B(0, &nand_reg->nfaddr); /*1st cycle*/

	res = __READ_B(&nand_reg->nfdata); 

	return ((res << 8) | __READ_B(&nand_reg->nfdata));
}



static int nand_read_init(struct boot_nand_t *mtd)
{
	struct s3c2440_nand *nand_reg = s3c2440_get_base_nand();
    struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	unsigned short nand_id;
    unsigned int cfg;
	int i;
	
    NFBT_PUTS("read init start...\n");
    
    /* Enable NAND flash clk */
    __WRITE_L(__READ_L(&clk_power->clkcon) | (1 << 4), &clk_power->clkcon);
    
    cfg = NFCONF_TACLS(1) | NFCONF_TWRPH0(2) | NFCONF_TWRPH1(0) | NFCONF_BUSWIDTH(0);
	__WRITE_L(cfg, &nand_reg->nfconf);
	
    NFBT_DEBUG("%x:%x/%x\n", &nand_reg->nfconf, __READ_L(&nand_reg->nfconf), cfg);
    
    cfg = NFCONT_MODE_BIT; /* wx:controler enable */
    cfg &= ~NFCONT_RnB_MODE;//wx: detect rise edge, 
#ifdef CONFIG_NAND_BOOT_SKIP_BAD
    cfg |= NFCONT_INITECC_BIT;//init ecc  
#endif
    cfg &= ~NFCONT_nCE_BIT;//chip enable,
	__WRITE_L(cfg, &nand_reg->nfcont);

    NFBT_DEBUG("%x:%x/%x\n", &nand_reg->nfcont, __READ_L(&nand_reg->nfcont), cfg);
    
    cfg = NFSTAT_RnB_BIT | NFSTAT_nCE_BIT;
	__WRITE_L(cfg, &nand_reg->nfstat);	/*wx: clr RnB*/

	NFBT_DEBUG("%x:%x/%x\n", &nand_reg->nfstat, __READ_L(&nand_reg->nfstat), cfg);
	//------------------------------------------------------------------
    
    
	__WRITE_B(NAND_CMD_RESET, &nand_reg->nfcmd); /*wx: reset mtd*/
    NFBT_DEBUG("%x:cmd:%x/%x\n", &nand_reg->nfcmd, __READ_B(&nand_reg->nfcmd), NAND_CMD_RESET);
    
    //wx: is NFSTAT_RnB_BIT is hight, means chip is ready
	while(!(__READ_B(&nand_reg->nfstat) & NFSTAT_RnB_BIT)) {
        for (i=0; i<10; i++);
        NFBT_DEBUG("RnB %x: %x\n", 
                   &nand_reg->nfstat,
                   __READ_B(&nand_reg->nfstat));
    }
    
	
	//------------------------------------------------------------------
	
	nand_enable_chip();
	nand_clear_chip_busy();
	i =  0;
    cfg = 0;
	nand_id = nand_read_id();

   if (nand_id == 0xec76 || 	/* Samsung K91208 */
		   nand_id == 0xad76) {	/*Hynix HY27US08121A*/
		mtd->page_size  = 512;
		cfg = (16 << 10);
#ifdef CONFIG_NAND_BOOT_SKIP_BAD
        mtd->block_size = cfg;
		mtd->bad_block_offset = 5;
#endif
	} else if (nand_id == 0xecf1 || /* Samsung K9F1G08U0B */
		   nand_id == 0xecda || /* Samsung K9F2G08U0B */
		   nand_id == 0xecd3 )	{ /* Samsung K9K8G08 */
		mtd->page_size = (2 << 10);
        cfg = (128 << 10);
#ifdef CONFIG_NAND_BOOT_SKIP_BAD
        mtd->block_size = cfg;
		mtd->bad_block_offset = mtd->page_size;
#endif
	} else {
		i = -1; // hang
	}
	
	nand_disable_chip();
    NFBT_DEBUG("NandInfo: ID=0x%x, page=%d, blk=%d\n", nand_id, 
               mtd->page_size, cfg);
	return i;
	
}

/* low level nand read function */
static int nand_read_ll(struct boot_nand_t *mtd, unsigned char *buf, unsigned long start_addr, int size)
{
	int i, ret;
    
#ifdef CONFIG_S3C2410_NAND_SKIP_BAD
	if ((start_addr & (mtd->block_size-1)) || (size & ((mtd->block_size-1)))) {
        NFBT_DEBUG("invalid alignment:%x:%x:%x\n", start_addr, 
                   mtd->block_size-1,  size);
        return -1;  /* invalid alignment */
    }
#endif		
		
	/* chip Enable */
	nand_enable_chip();
	nand_clear_chip_busy();
	
	for (i = 0; i < 10; i++)
		;
    
    ret = 0;
	for (i=start_addr; i < (start_addr + size);) {
#ifdef CONFIG_NAND_BOOT_SKIP_BAD
		if (i & (mtd->block_size-1)== 0) {
			if (is_bad_block(mtd, i) ||
			    is_bad_block(mtd, i + mtd->page_size)) {
				/* Bad block */
				i += mtd->block_size;
				size += mtd->block_size;
				continue;
			}
		}
#endif
		
        if (0 == nand_read_page_ll(mtd, buf, i)) {
            i += mtd->page_size;
            buf += mtd->page_size;
        } else {
            ret = -1;
            break;
        }
	}

	/* chip Disable */
	nand_disable_chip();

	return ret;
}

#ifdef NAND_BOOT_LOAD_VERIFY
static int nandboot_imgcmp(const void * cs,const void * ct,size_t count)
{
	const unsigned int *su1, *su2;

	count >>= 2;
	
	for(su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if (*su1 !=  *su2)
			return -1;
			
	return 0;
}
#endif

int nandboot_load_img(unsigned char * destAddr)
{
	struct boot_nand_t nand;
    
   NFBT_DEBUG("nandboot_load_img(dest 0+%x-> %x)...\n", 
           NANDBOOT_IMG_LENGTH, destAddr);
   
    // init nand
	if (0 != nand_read_init(&nand))	{
           NFBT_DEBUG("nand_read_init failed(%d)\n", nand.page_size);
           return -1; 
    }
    
    NFBT_PUTS("init ok..");

    // load..
    if (0 != nand_read_ll(&nand, destAddr, 0, NANDBOOT_IMG_LENGTH)) {
            NFBT_PUTS("nandboot_nand_read_ll failed\n");
            return -1; 
    }

    NFBT_PUTS("nandboot_load_load_successful\n");
    
#ifdef NAND_BOOT_LOAD_VERIFY
    // verify
	if (0 != nandboot_imgcmp(destAddr, CONFIG_BOOT_INNER_SRAM_BASE, CONFIG_BOOT_INNER_SRAM_SIZE))
		return -1;
#endif
    
	return  0;
}
#endif

