/*
 * Copyright (C) 2007 Google, Inc.
 * Copyright (c) 2008-2010, Code Aurora Forum. All rights reserved.
 * Author: Brian Swetland <swetland@google.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *
 * The MSM peripherals are spread all over across 768MB of physical
 * space, which makes just having a simple IO_ADDRESS macro to slide
 * them into the right virtual location rough.  Instead, we will
 * provide a master phys->virt mapping for peripherals here.
 *
 */

#ifndef __ASM_ARCH_MSM_IOMAP_7X30_H
#define __ASM_ARCH_MSM_IOMAP_7X30_H

/* Physical base address and size of peripherals.
 * Ordered by the virtual base addresses they will be mapped at.
 *
 * MSM_VIC_BASE must be an value that can be loaded via a "mov"
 * instruction, otherwise entry-macro.S will not compile.
 *
 * If you add or remove entries here, you'll want to edit the
 * msm_io_desc array in arch/arm/mach-msm/io.c to reflect your
 * changes.
 *
 */

/* ========================================================================================
when                who               what, where, why                comment tag
--------         ----       -----------------------------             --------------------------
2011-02-11    rjg           add f3 log support          ZTE_F3LOG_RJG_20110211
==========================================================================================*/
#define MSM_VIC_BASE          IOMEM(0xFA000000)
#define MSM_VIC_PHYS          0xC0080000
#define MSM_VIC_SIZE          SZ_4K

#define MSM_CSR_BASE          IOMEM(0xFA001000)
#define MSM_CSR_PHYS          0xC0100000
#define MSM_CSR_SIZE          SZ_4K

#define MSM_TMR_PHYS          MSM_CSR_PHYS
#define MSM_TMR_BASE          MSM_CSR_BASE
#define MSM_TMR_SIZE          SZ_4K

#define MSM_DMOV_BASE         IOMEM(0xFA002000)
#define MSM_DMOV_PHYS         0xAC400000
#define MSM_DMOV_SIZE         SZ_4K

#define MSM_GPIO1_BASE        IOMEM(0xFA003000)
#define MSM_GPIO1_PHYS        0xAC001000
#define MSM_GPIO1_SIZE        SZ_4K

#define MSM_GPIO2_BASE        IOMEM(0xFA004000)
#define MSM_GPIO2_PHYS        0xAC101000
#define MSM_GPIO2_SIZE        SZ_4K

#define MSM_CLK_CTL_BASE      IOMEM(0xFA005000)
#define MSM_CLK_CTL_PHYS      0xAB800000
#define MSM_CLK_CTL_SIZE      SZ_4K

#define MSM_CLK_CTL_SH2_BASE  IOMEM(0xFA006000)
#define MSM_CLK_CTL_SH2_PHYS  0xABA01000
#define MSM_CLK_CTL_SH2_SIZE  SZ_4K

#define MSM_ACC_BASE          IOMEM(0xFA007000)
#define MSM_ACC_PHYS          0xC0101000
#define MSM_ACC_SIZE          SZ_4K

#define MSM_SAW_BASE          IOMEM(0xFA008000)
#define MSM_SAW_PHYS          0xC0102000
#define MSM_SAW_SIZE          SZ_4K

#define MSM_GCC_BASE	      IOMEM(0xFA009000)
#define MSM_GCC_PHYS	      0xC0182000
#define MSM_GCC_SIZE	      SZ_4K

#define MSM_TCSR_BASE	      IOMEM(0xFA00A000)
#define MSM_TCSR_PHYS	      0xAB600000
#define MSM_TCSR_SIZE	      SZ_4K

#define MSM_SHARED_RAM_BASE   IOMEM(0xFA100000)
#define MSM_SHARED_RAM_SIZE   SZ_1M

#define MSM_UART1_PHYS        0xACA00000
#define MSM_UART1_SIZE        SZ_4K

#define MSM_UART2_PHYS        0xACB00000
#define MSM_UART2_SIZE        SZ_4K

#define MSM_UART3_PHYS        0xACC00000
#define MSM_UART3_SIZE        SZ_4K

#ifdef CONFIG_MSM_DEBUG_UART
#define MSM_DEBUG_UART_BASE   0xFB000000
#if CONFIG_MSM_DEBUG_UART == 1
#define MSM_DEBUG_UART_PHYS   MSM_UART1_PHYS
#elif CONFIG_MSM_DEBUG_UART == 2
#define MSM_DEBUG_UART_PHYS   MSM_UART2_PHYS
#elif CONFIG_MSM_DEBUG_UART == 3
#define MSM_DEBUG_UART_PHYS   MSM_UART3_PHYS
#endif
#define MSM_DEBUG_UART_SIZE   SZ_4K
#endif

#define MSM_MDC_BASE	      IOMEM(0xFA200000)
#define MSM_MDC_PHYS	      0xAA500000
#define MSM_MDC_SIZE	      SZ_1M

#define MSM_AD5_BASE          IOMEM(0xFA300000)
#define MSM_AD5_PHYS          0xA7000000
#define MSM_AD5_SIZE          (SZ_1M*13)

//ZTE_SMEMLOG_WXW_20101015, begin
#define MSM_SMEM_RAM_PHYS    0xFE00000
#define MSM_SMEM_RAM_SIZE     (SZ_1M)
//#define MSM_SMEM_RAM_SIZE     (SZ_1M-SZ_4K)

//#define MSM_BAT_RAM_PHYS    0xFEFF000
//#define MSM_BAT_RAM_SIZE     SZ_4K
//ZTE_SMEMLOG_WXW_20101015, end

#if defined(CONFIG_ZTE_PLATFORM) && defined(CONFIG_F3_LOG)
/* ZTE_F3LOG_RJG_20110211 begin */
#define MSM_RAM_LOG_BASE      IOMEM(0xFB600000)
#define MSM_RAM_LOG_SIZE   	  SZ_1M
/* ZTE_F3LOG_RJG_20110211 end */
#endif


#endif
