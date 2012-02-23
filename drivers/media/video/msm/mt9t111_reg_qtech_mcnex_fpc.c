/*
 * drivers/media/video/msm/mt9t111_reg_qtech.c
 *
 * Refer to drivers/media/video/msm/mt9d112_reg.c
 * For MT9T111: 3.1Mp, 1/4-Inch System-On-A-Chip (SOC) CMOS Digital Image Sensor
 *
 * Copyright (C) 2009-2010 ZTE Corporation.
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
 * Created by jia.jia@zte.com.cn
 */
/*-----------------------------------------------------------------------------------------
  when         who          what, where, why                         comment tag
  --------     ----         -------------------------------------    ----------------------
  2010-12-15   jia.jia      add support for exposure compensation    ZTE_MSM_CAMERA_JIA_20101215
  2010-07-21   jia          fix bug of wrong ptr of                  ZTE_MSM_CAMERA_JIA_001
                            "lens_for_indoor_tbl"
  2010-04-23   ygl          add process for setting WB               ZTE_MSM_CAMERA_YGL_001
                            successfully when returned from other
                            effect mode
  2010-04-15   zh.shj       add process for different board types    ZTE_MSM_CAMERA_ZHSHJ_001
                            e.g., CONFIG_MACH_BLADE and CONFIG_
                            MACH_RAISE
  2010-01-14   zh.shj       modify config of sharpness               ZTE_MSM_CAMERA_ZHSHJ_001
  2010-01-07   zh.shj       modify config of saturation to fix bug   ZTE_MSM_CAMERA_ZHSHJ_001
                            of no effect after setting effect
  2010-01-06   zh.shj       modify config of PLL to make PCLK equal  ZTE_MSM_CAMERA_ZHSHJ_001
                            to MCLK
  2010-01-05   zh.shj       modify config of brightness              ZTE_MSM_CAMERA_ZHSHJ_001
  2009-12-21   zh.shj       improve effects of white balance         ZTE_MSM_CAMERA_ZHSHJ_001
  2009-12-21   chg          for vfe zoom                             CHG_CAM_20091217
  2009-12-19   zh.shj       add config for contrast                  ZTE_MSM_CAMERA_ZHSHJ_001
  2009-12-11   jia.jia      rename file with mt9t111_reg_qtech       ZTE_MSM_CAMERA_JIA_001
  2009-12-03   jia.jia      Improve efficiency of sensor init        ZTE_MSM_CAMERA_JIA_001
  2009-12-01   jia.jia      Refactor code for sensor init            ZTE_MSM_CAMERA_JIA_001
  2009-11-09   jia.jia      add wb and af settings                   ZTE_MSM_CAMERA_ZHSHJ_001
  2009-11-26   zh.shj       improve effects of AF, AWB and LSC;      ZTE_MSM_CAMERA_ZHSHJ_001
  2009-11-09   jia.jia      add orientation setting for preview      ZTE_MSM_CAMERA_ZHSHJ_001
                            and snapshot
  2009-11-05   zh.shj       mt9t111 preview function implemented     ZTE_MSM_CAMERA_ZHSHJ_001
  2009-10-24   jia.jia      Merged from kernel-v4515                 ZTE_MSM_CAMERA_JIA_001
------------------------------------------------------------------------------------------*/

#include "mt9t11x.h"

/* PLL Setup */
static struct mt9t11x_i2c_reg_conf const pll_tbl[] = {
    {0x0014, 0xB047, WORD_LEN, 0},   /* PLL_CONTROL */
    {0x0014, 0xB045, WORD_LEN, 0},   /* PLL_CONTROL */
    {0x0014, 0x2145, WORD_LEN, 0},   /* PLL_CONTROL */
    {0x0010, 0x0310, WORD_LEN, 0},   /* PLL_DIVIDERS */
    {0x0012, 0x0070, WORD_LEN, 0},   /* PLL_P_DIVIDERS */
    {0x002A, 0x7788, WORD_LEN, 0},   /* PLL_P4_P5_P6_DIVIDERS */
    {0x0014, 0x2545, WORD_LEN, 0},   /* PLL_CONTROL */
    {0x0014, 0x2547, WORD_LEN, 0},   /* PLL_CONTROL */
    {0x0014, 0x3447, WORD_LEN, 0},   /* PLL_CONTROL */
    {0x0014, 0x3047, WORD_LEN, 10},  /* PLL_CONTROL */
};

/* Clock Setup */
static struct mt9t11x_i2c_reg_conf const clk_tbl[] = {
    {0x0014, 0x3046, WORD_LEN, 0},   /* PLL control: PLL_BYPASS off = 12358 */
    {0x0022, 0x0320, WORD_LEN, 0},   /* Reference clock count for 20 us = 800 */
    {0x001E, 0x0707, WORD_LEN, 0},   /* Pad Slew Rate = 1799 */
    {0x3B84, 0x006A, WORD_LEN, 0},   /* I2C Master Clock Divider = 363  1AA */
    {0x3C20, 0x0001, WORD_LEN, 0},   /* TX_SS_CONTROL, 0x0000 to 0x0001 */
    {0x0016, 0x0400, WORD_LEN, 0},   /* CLOCKS_CONTROL */
    {0x0018, 0x402D, WORD_LEN, 10},  /* STANDBY_CONTROL_AND_STATUS */
};

/* Preview and Snapshot Setup */
static struct mt9t11x_i2c_reg_conf const prev_snap_tbl[] = {
    //save power mode
    {0x3084, 0x2409, WORD_LEN, 0},   // RESERVED_CORE_3084
    {0x3092, 0x0A49, WORD_LEN, 0},   // RESERVED_CORE_3092
    {0x3094, 0x4949, WORD_LEN, 0},   // RESERVED_CORE_3094
    {0x3096, 0x4950, WORD_LEN, 0},   // RESERVED_CORE_3096
    //AE
    {0x098E, 0x6C25, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0008, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0x6C27, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xE82C, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0001, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0x682F, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0080, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0x4815, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0001, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0x485D, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0001, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0x681B, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0004, WORD_LEN, 0},   // MCU_DATA_0

    /*
      * Refactor code for sensor init
      */
    //PATCH
    {0x0982, 0x0000, WORD_LEN, 0},   // ACCESS_CTL_STAT
    {0x098A, 0x0351, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x3C3C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC640, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xF730, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0BED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFD30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC0FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0BEF, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0361, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC2DE, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x1DEC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x25FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C5, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0101, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x01FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C2, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0371, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xD6EC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0C5F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x8402, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x4416, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x4F30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xE703, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xF10B, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0381, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xF127, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x15F1, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0BF1, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x2309, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFC06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xD604, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xD620, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0391, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x07FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06D6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x05FD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06D6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xDE1D, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xE60B, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC407, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30E7, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x03A1, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x02F1, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CC6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x272C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x7D0C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC627, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x275D, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2724, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x7F30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x03B1, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06D6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC5F6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0CC6, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x4FFD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C7, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC640, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x03C1, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xF730, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC4E6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x024F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD30, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC501, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0101, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFC30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x03D1, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x06D6, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x7D06, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xCB27, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x2EC6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x40F7, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C4, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFC06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC104, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x03E1, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xF306, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xD6ED, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x005F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x6D00, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x2A01, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x5317, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC0EC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x03F1, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x00FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C2, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC06, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC1FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C5, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0101, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x01FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C2, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0401, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC720, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x227F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x30C4, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xDE1D, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xEC25, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC5FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0411, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x06D6, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD30, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC701, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC30, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06D0, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFC30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0421, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x06D2, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xEC25, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD06, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC3BD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x953C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xDE3F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xEE10, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xAD00, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0431, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x3838, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x393C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x3CBD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x61D5, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCE04, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xCD1F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x1702, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x11CC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0441, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x332E, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30ED, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x02CC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFFFD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xED00, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xCC00, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x02BD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x706D, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0451, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x18DE, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x1F18, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x1F8E, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0110, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCC3C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x5230, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xED00, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x18EC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0461, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xA0C4, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFDBD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x7021, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x201E, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCC3C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x5230, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xED00, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0471, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xECA0, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xBD70, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x21CC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x3C52, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30ED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x02CC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFFFC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xED00, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0481, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xCC00, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x02BD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x706D, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x3838, WORD_LEN, 0},   // MCU_DATA_3
    {0x0982, 0x0000, WORD_LEN, 0},   // ACCESS_CTL_STAT
    {0x098A, 0x8489, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0039, WORD_LEN, 0},   // MCU_DATA_0
    {0x098A, 0x0AE0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x37DE, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x1DEC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0C5F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x8402, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x4416, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x4FF7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0BF1, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xE60B, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0AF0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC407, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xF70C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC67F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x30C4, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xEC25, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFD30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC5FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06D6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0B00, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC701, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0BED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0BEF, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0B10, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30E6, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x00BD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x5203, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x3139, WORD_LEN, 0},   // MCU_DATA_3
    {0x098A, 0x0CC7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x3C3C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x3C3C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC06, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xD230, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xED06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC06, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xD0ED, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x04FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0CD7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CC0, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED02, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xBEED, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x00BD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xD4D0, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2356, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0CE7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xBEFD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06D0, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x06D2, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC06, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xD0FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0CF7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xD2FD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x8640, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xB730, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFD30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D07, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC0FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC2FC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0CC4, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFD30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC501, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x017F, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D17, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30C4, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xBCFD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x30C5, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFC30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C7, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x01FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D27, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30C2, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xBC7C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06CB, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFE06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC708, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFF06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC77A, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D37, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x202F, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xEC31, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30ED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x065F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x4FED, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x04FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D47, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x06D2, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED02, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC06, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xD0ED, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x00BD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xD4D0, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2312, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xCE06, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D57, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xAD1C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0101, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x5F4F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xD0EC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x31FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06D2, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D67, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x7F0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xBA38, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x3838, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x3839, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x3C3C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x3C3C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x7F30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D77, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x06CC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD30, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC5F6, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0CBB, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x4FFD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC640, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xF730, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D87, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC501, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0101, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0D97, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xEC33, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30ED, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x065F, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x4FED, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0DA7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x04FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xED02, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xB6ED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x00BD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xD4D0, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x2331, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0DB7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x7F30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC4DE, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x1FEC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x33FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C5, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xBCFD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C7, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0DC7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC640, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xF730, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06CC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC501, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0101, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0DD7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xEC33, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD06, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCC7A, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06CB, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x7A0C, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xBA20, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0DE7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x35DE, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x1FEC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x3530, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xED06, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x5F4F, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xED04, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xB8ED, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0DF7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x02FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xED00, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xBDD4, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xD024, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x12DE, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x1F5F, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x4FFD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E07, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xEC35, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xB8CE, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x06AD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x1C01, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x02FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E17, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xCC7F, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x3838, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x3838, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x39C6, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x40F7, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C4, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E27, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xB6FD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C0, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xB8FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C2, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xEC37, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E37, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC501, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0101, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC30, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06C7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xEC3B, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xB306, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E47, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC723, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x427F, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C4, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xBCFD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C5, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xEC37, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFD30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E57, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC7C6, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x40F7, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C4, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC4FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C5, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0101, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x01FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E67, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30C2, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xBCEC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x37FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x06CE, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xBEFD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06D0, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E77, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x06D2, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xEC3B, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x8300, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x01FD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x06C7, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x7A06, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E87, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xCB7A, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x39C6, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x40F7, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C4, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xB6FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C0, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0E97, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xB8FD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C2, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC06, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC7FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C5, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0101, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x01FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0EA7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30C2, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD06, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xCE7F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x39FC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06C5, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC786, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0EB7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x40B7, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C4, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xB6FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C0, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xB8FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C2, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0EC7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC7FD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C5, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0101, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x01FC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C2, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xCEDE, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0ED7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x1FEC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x39B3, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x06CE, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x2324, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC640, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xF730, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0EE7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC0FC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD30, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC2FC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06C5, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC501, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0EF7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0101, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFC30, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06CE, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x7F0C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xBA39, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xEC39, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F07, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xCE7F, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C4, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xEC39, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFE06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC5BD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xD479, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFD30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F17, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC5FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC7C6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x40F7, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C4, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC4FD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F27, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30C5, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0101, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x01FC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x30C2, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xBC7C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x06CB, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x7A0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F37, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xBA39, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x3CBD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x776D, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xCC32, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x5C30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xED00, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFC13, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x8683, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F47, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0001, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xBD70, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x21CC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x325E, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30ED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x00FC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x1388, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x8300, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F57, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x01BD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x7021, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x3839, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x308F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC3FF, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xE98F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x35BD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xAD15, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F67, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x306F, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x1618, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xDE19, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x188F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC301, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x4B18, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x8F18, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xEC00, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F77, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD05, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0E18, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xEC02, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD05, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x10E6, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x164F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xED04, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x188F, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F87, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC3FF, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xCBE3, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x048F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xE600, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xF705, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x1418, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xDE19, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30E6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0F97, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x164F, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED04, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x188F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC301, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x19E3, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x048F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xE600, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xF705, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0FA7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x15FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x055B, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD05, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x12DE, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x37EE, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x08AD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0030, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xE616, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0FB7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x4F05, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED04, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x8FC3, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0006, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30E3, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x048F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xF605, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x164F, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0FC7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xED00, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x306C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x16E6, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x16C1, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0325, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x98CC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x328E, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xED00, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0FD7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xEC06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xBD70, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x21CC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x326C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30ED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x02CC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xF800, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xED00, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x0FE7, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xA609, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xE30A, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x8407, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xBD70, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x6D30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC617, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x3A35, WORD_LEN, 0},   // MCU_DATA_6
    {0x0982, 0x0000, WORD_LEN, 0},   // ACCESS_CTL_STAT
    {0x098A, 0x8FF5, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0039, WORD_LEN, 0},   // MCU_DATA_0
    {0x098A, 0x1000, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xCC10, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x09BD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x4224, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x7E10, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x09C6, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x26F7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x018A, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xDE3F, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1010, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x18CE, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0BF2, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xCC00, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x11BD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xD700, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xCC0B, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xF2DD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x3FDE, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1020, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x3518, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xCE0C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x04CC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x003F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xBDD7, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x00CC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0C04, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xDD35, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1030, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xDE47, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x18CE, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0C44, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xCC00, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x15BD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xD700, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xCC0C, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x44DD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1040, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x47DE, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x4118, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xCE0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x5ACC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x000D, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xBDD7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x00CC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0C5A, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1050, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xDD41, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xDE39, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x18CE, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0C68, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCC00, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x23BD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xD700, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xCC0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1060, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x68DD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x39FE, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0031, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x18CE, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0C8C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xCC00, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x29BD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xD700, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1070, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xCC0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x8CFD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0031, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xCC03, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x51FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0BFE, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xCC10, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xA4FD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1080, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0BFC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xCC04, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x34FD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0C20, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCC0F, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x5DFD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0C52, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xCC10, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1090, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xA1FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0C5C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xCC0F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x39FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0C74, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xCC0A, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xE0FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0C96, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x10A0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x397E, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x9E14, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x308F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC3FF, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xF08F, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x35F6, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x06BC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x4FFD, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x10B0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x7F30, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06C1, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC5FC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x06C3, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0404, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x10C0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC701, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0CBE, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0CC0, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x10D0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xD2ED, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x02FC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06D0, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xED00, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC602, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xBDD6, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xE030, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x10E0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xEC00, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD06, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xD0EC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x02FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x06D2, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC604, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xF70C, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xBAFC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x10F0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x2606, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xCC00, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x01FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x7D0C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xBC27, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x07C6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1100, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFF30, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xE70C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x2006, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xF60C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xBD30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xE70C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xF606, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xBA54, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1110, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x5454, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x54E7, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0D6C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0DF6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x06C9, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC101, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x260B, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xF606, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1120, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xBAE7, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0EE6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x0CE7, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0F20, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x09E6, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0CE7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0EF6, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06BA, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1130, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xE70F, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xE60E, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xE10F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x2206, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x7F06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC97E, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x1339, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xE60E, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1140, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xE00F, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xE70F, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xE62C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x4F30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xED0A, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xE60F, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x4FEE, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1150, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0A02, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x8F30, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xE70F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xE10D, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x2404, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xE60D, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xE70F, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xF606, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1160, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC9C1, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0126, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x09E6, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0CEB, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0FF7, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0CBB, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2007, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xE60C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1170, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xE00F, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xF70C, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xBBF6, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x06CB, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x270D, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC101, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x275D, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC102, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1180, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x2603, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x7E13, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x2E20, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x51FC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x06D0, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC2FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06D2, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1190, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC486, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x40B7, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x30C4, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C0, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x11A0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC4FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C2, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xBCFD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C5, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0101, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x7F30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC4F6, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x11B0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CBB, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x4FFD, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C5, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFC30, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x30C7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x01FC, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C0, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x11C0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xB6FC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C2, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xB8FC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xD0FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x11D0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFD06, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xD2BD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0CC7, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x7E13, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x317F, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C4, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC06, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x11E0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC7FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C5, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC06, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xCEFD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C7, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x01FC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C0, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x11F0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC2FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C2, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC47F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C4, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFD30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC5F6, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x0CBB, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1200, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x4FFD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C7, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC640, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xF730, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0CBC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD30, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xC501, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1210, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0101, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xFC30, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC0FD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFC30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1220, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x3C18, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x38EC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x2DCD, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xEE37, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xBDD4, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x7930, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xED0A, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x5F6D, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1230, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0A2A, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0153, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x17ED, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x08EC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0AED, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06EC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x08ED, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x04FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1240, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED02, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xB6ED, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x00BD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xD4D0, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2305, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xBD0E, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1250, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x2220, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x85DE, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x1F3C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x1838, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xEC2D, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xCDEE, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2FBD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xD479, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1260, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x30ED, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0A5F, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x6D0A, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x2A01, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x5317, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xED08, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xEC0A, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xED06, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1270, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xEC08, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED04, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xB8ED, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x02FC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xED00, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xBDD4, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1280, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xD024, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x72C6, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x40F7, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x30C4, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xB6FD, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C0, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1290, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xB8FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C2, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xDE1F, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xEC2F, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC501, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0101, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x12A0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06C7, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC727, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x07FE, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06C7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x08FF, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06C7, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x12B0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC640, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xF730, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC4FC, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x0CB6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD30, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC0FC, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0CB8, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFD30, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x12C0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC2FC, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06C7, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFD30, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xC501, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x0101, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC30, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC2FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x06CE, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x12D0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xFC06, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xC7B3, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x06C5, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x2305, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xBD0E, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xB020, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x54DE, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x1FEC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x12E0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x3BB3, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06C7, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x230A, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xEC3B, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xFD06, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xC77A, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x2041, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x12F0, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x7F0C, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xBA20, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x3CDE, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x1FEC, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x2DFD, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06C7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xEC2D, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xB306, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1300, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xC523, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06FC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x06C5, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD06, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC7C6, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x40F7, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30C4, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1310, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0xB6FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x30C0, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xFC0C, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xB8FD, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x30C2, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xFC06, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xC7FD, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x30C5, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1320, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x0101, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x01FC, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C2, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xFD06, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xCE7F, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x2003, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xBD0D, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1330, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x6F7D, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x0CBA, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x2703, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x7E10, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xEFFC, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0x06D2, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0x30ED, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0x02FC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1340, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x06D0, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0xED00, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0xC602, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0xBDD6, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0xC030, WORD_LEN, 0},   // MCU_DATA_4
    {0x099A, 0xEC00, WORD_LEN, 0},   // MCU_DATA_5
    {0x099C, 0xFD06, WORD_LEN, 0},   // MCU_DATA_6
    {0x099E, 0xD0EC, WORD_LEN, 0},   // MCU_DATA_7
    {0x098A, 0x1350, WORD_LEN, 0},   // PHYSICAL_ADDR_ACCESS
    {0x0990, 0x02FD, WORD_LEN, 0},   // MCU_DATA_0
    {0x0992, 0x06D2, WORD_LEN, 0},   // MCU_DATA_1
    {0x0994, 0x30C6, WORD_LEN, 0},   // MCU_DATA_2
    {0x0996, 0x103A, WORD_LEN, 0},   // MCU_DATA_3
    {0x0998, 0x3539, WORD_LEN, 0},   // MCU_DATA_4
    {0x098E, 0x0010, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x1000, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0x0003, WORD_LEN, 0},   // MCU_ADDRESS
    {0x0990, 0x0004, WORD_LEN, 0},   // MCU_DATA_0
    //[MICRON GOLDEN LSC 20080627]

    //lens shading
    {0x364A, 0x0110, WORD_LEN, 0}, 	// P_R_P0Q0
    {0x364C, 0x570C, WORD_LEN, 0}, 	// P_R_P0Q1
    {0x364E, 0x5FF1, WORD_LEN, 0}, 	// P_R_P0Q2
    {0x3650, 0x9DAE, WORD_LEN, 0}, 	// P_R_P0Q3
    {0x3652, 0x8411, WORD_LEN, 0}, 	// P_R_P0Q4
    {0x368A, 0x9A6C, WORD_LEN, 0}, 	// P_R_P1Q0
    {0x368C, 0x32AF, WORD_LEN, 0}, 	// P_R_P1Q1
    {0x368E, 0x55CE, WORD_LEN, 0}, 	// P_R_P1Q2
    {0x3690, 0xF56F, WORD_LEN, 0}, 	// P_R_P1Q3
    {0x3692, 0xDC6F, WORD_LEN, 0}, 	// P_R_P1Q4
    {0x36CA, 0x2052, WORD_LEN, 0}, 	// P_R_P2Q0
    {0x36CC, 0x8530, WORD_LEN, 0}, 	// P_R_P2Q1
    {0x36CE, 0xA153, WORD_LEN, 0}, 	// P_R_P2Q2
    {0x36D0, 0x98F0, WORD_LEN, 0}, 	// P_R_P2Q3
    {0x36D2, 0x04B4, WORD_LEN, 0}, 	// P_R_P2Q4
    {0x370A, 0x800C, WORD_LEN, 0}, 	// P_R_P3Q0
    {0x370C, 0xE42F, WORD_LEN, 0}, 	// P_R_P3Q1
    {0x370E, 0x23EE, WORD_LEN, 0}, 	// P_R_P3Q2
    {0x3710, 0x0DEF, WORD_LEN, 0}, 	// P_R_P3Q3
    {0x3712, 0x2694, WORD_LEN, 0}, 	// P_R_P3Q4
    {0x374A, 0xA0D3, WORD_LEN, 0}, 	// P_R_P4Q0
    {0x374C, 0x5AD1, WORD_LEN, 0}, 	// P_R_P4Q1
    {0x374E, 0x8115, WORD_LEN, 0}, 	// P_R_P4Q2
    {0x3750, 0x56D5, WORD_LEN, 0}, 	// P_R_P4Q3
    {0x3752, 0x20F8, WORD_LEN, 0}, 	// P_R_P4Q4
    {0x3640, 0x0630, WORD_LEN, 0}, 	// P_G1_P0Q0
    {0x3642, 0x3D6D, WORD_LEN, 0}, 	// P_G1_P0Q1
    {0x3644, 0x70B1, WORD_LEN, 0}, 	// P_G1_P0Q2
    {0x3646, 0xA8CE, WORD_LEN, 0}, 	// P_G1_P0Q3
    {0x3648, 0xEBF1, WORD_LEN, 0}, 	// P_G1_P0Q4
    {0x3680, 0xDB4B, WORD_LEN, 0}, 	// P_G1_P1Q0
    {0x3682, 0xAC6E, WORD_LEN, 0}, 	// P_G1_P1Q1
    {0x3684, 0x806D, WORD_LEN, 0}, 	// P_G1_P1Q2
    {0x3686, 0x66EF, WORD_LEN, 0}, 	// P_G1_P1Q3
    {0x3688, 0x768E, WORD_LEN, 0}, 	// P_G1_P1Q4
    {0x36C0, 0x1F32, WORD_LEN, 0}, 	// P_G1_P2Q0
    {0x36C2, 0xEE10, WORD_LEN, 0}, 	// P_G1_P2Q1
    {0x36C4, 0xE7B3, WORD_LEN, 0}, 	// P_G1_P2Q2
    {0x36C6, 0x530D, WORD_LEN, 0}, 	// P_G1_P2Q3
    {0x36C8, 0x7F14, WORD_LEN, 0}, 	// P_G1_P2Q4
    {0x3700, 0x618B, WORD_LEN, 0}, 	// P_G1_P3Q0
    {0x3702, 0xD991, WORD_LEN, 0}, 	// P_G1_P3Q1
    {0x3704, 0x628F, WORD_LEN, 0}, 	// P_G1_P3Q2
    {0x3706, 0x74F3, WORD_LEN, 0}, 	// P_G1_P3Q3
    {0x3708, 0x4D14, WORD_LEN, 0}, 	// P_G1_P3Q4
    {0x3740, 0x9FB3, WORD_LEN, 0}, 	// P_G1_P4Q0
    {0x3742, 0x09D3, WORD_LEN, 0}, 	// P_G1_P4Q1
    {0x3744, 0xFD73, WORD_LEN, 0}, 	// P_G1_P4Q2
    {0x3746, 0x26D5, WORD_LEN, 0}, 	// P_G1_P4Q3
    {0x3748, 0x1198, WORD_LEN, 0}, 	// P_G1_P4Q4
    {0x3654, 0x00F0, WORD_LEN, 0}, 	// P_B_P0Q0
    {0x3656, 0x156D, WORD_LEN, 0}, 	// P_B_P0Q1
    {0x3658, 0x45F1, WORD_LEN, 0}, 	// P_B_P0Q2
    {0x365A, 0xC84D, WORD_LEN, 0}, 	// P_B_P0Q3
    {0x365C, 0xEBD0, WORD_LEN, 0}, 	// P_B_P0Q4
    {0x3694, 0xF36A, WORD_LEN, 0}, 	// P_B_P1Q0
    {0x3696, 0x99EE, WORD_LEN, 0}, 	// P_B_P1Q1
    {0x3698, 0xBC8C, WORD_LEN, 0}, 	// P_B_P1Q2
    {0x369A, 0x754F, WORD_LEN, 0}, 	// P_B_P1Q3
    {0x369C, 0xBA27, WORD_LEN, 0}, 	// P_B_P1Q4
    {0x36D4, 0x1692, WORD_LEN, 0}, 	// P_B_P2Q0
    {0x36D6, 0xA92E, WORD_LEN, 0}, 	// P_B_P2Q1
    {0x36D8, 0xADD3, WORD_LEN, 0}, 	// P_B_P2Q2
    {0x36DA, 0x8F71, WORD_LEN, 0}, 	// P_B_P2Q3
    {0x36DC, 0x2D74, WORD_LEN, 0}, 	// P_B_P2Q4
    {0x3714, 0x7E2D, WORD_LEN, 0}, 	// P_B_P3Q0
    {0x3716, 0xB2B0, WORD_LEN, 0}, 	// P_B_P3Q1
    {0x3718, 0xB9B1, WORD_LEN, 0}, 	// P_B_P3Q2
    {0x371A, 0x0293, WORD_LEN, 0}, 	// P_B_P3Q3
    {0x371C, 0x0415, WORD_LEN, 0}, 	// P_B_P3Q4
    {0x3754, 0x81F3, WORD_LEN, 0}, 	// P_B_P4Q0
    {0x3756, 0xA972, WORD_LEN, 0}, 	// P_B_P4Q1
    {0x3758, 0xD3D4, WORD_LEN, 0}, 	// P_B_P4Q2
    {0x375A, 0x7215, WORD_LEN, 0}, 	// P_B_P4Q3
    {0x375C, 0x0A58, WORD_LEN, 0}, 	// P_B_P4Q4
    {0x365E, 0x0170, WORD_LEN, 0}, 	// P_G2_P0Q0
    {0x3660, 0x06ED, WORD_LEN, 0}, 	// P_G2_P0Q1
    {0x3662, 0x5E51, WORD_LEN, 0}, 	// P_G2_P0Q2
    {0x3664, 0x9F0E, WORD_LEN, 0}, 	// P_G2_P0Q3
    {0x3666, 0xDF91, WORD_LEN, 0}, 	// P_G2_P0Q4
    {0x369E, 0xDF0B, WORD_LEN, 0}, 	// P_G2_P1Q0
    {0x36A0, 0x392F, WORD_LEN, 0}, 	// P_G2_P1Q1
    {0x36A2, 0x1BAC, WORD_LEN, 0}, 	// P_G2_P1Q2
    {0x36A4, 0xCB0F, WORD_LEN, 0}, 	// P_G2_P1Q3
    {0x36A6, 0x1A8D, WORD_LEN, 0}, 	// P_G2_P1Q4
    {0x36DE, 0x29F2, WORD_LEN, 0}, 	// P_G2_P2Q0
    {0x36E0, 0x8AD1, WORD_LEN, 0}, 	// P_G2_P2Q1
    {0x36E2, 0xE3B3, WORD_LEN, 0}, 	// P_G2_P2Q2
    {0x36E4, 0x14D1, WORD_LEN, 0}, 	// P_G2_P2Q3
    {0x36E6, 0x7C34, WORD_LEN, 0}, 	// P_G2_P2Q4
    {0x371E, 0xBD8B, WORD_LEN, 0}, 	// P_G2_P3Q0
    {0x3720, 0xC96F, WORD_LEN, 0}, 	// P_G2_P3Q1
    {0x3722, 0x6791, WORD_LEN, 0}, 	// P_G2_P3Q2
    {0x3724, 0x872F, WORD_LEN, 0}, 	// P_G2_P3Q3
    {0x3726, 0x3E73, WORD_LEN, 0}, 	// P_G2_P3Q4
    {0x375E, 0xB553, WORD_LEN, 0}, 	// P_G2_P4Q0
    {0x3760, 0x47F3, WORD_LEN, 0}, 	// P_G2_P4Q1
    {0x3762, 0x9593, WORD_LEN, 0}, 	// P_G2_P4Q2
    {0x3764, 0x2FF4, WORD_LEN, 0}, 	// P_G2_P4Q3
    {0x3766, 0x0158, WORD_LEN, 0}, 	// P_G2_P4Q4
    {0x3784, 0x0404, WORD_LEN, 0}, 	// CENTER_COLUMN
    {0x3782, 0x0304, WORD_LEN, 0}, 	// CENTER_ROW
    {0x3210, 0x00B8, WORD_LEN, 0}, 	// COLOR_PIPELINE_CONTROL

    //AWB and CCM
    {0x098E, 0x48B0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_0]
    {0x0990, 0x0143, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_1]
    {0x0990, 0xFFAB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_2]
    {0x0990, 0x0024, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_3]
    {0x0990, 0xFFC6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_4]
    {0x0990, 0x0167, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_5]
    {0x0990, 0xFFE5, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_6]
    {0x0990, 0x000E, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_7]
    {0x0990, 0xFEBD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_8]
    {0x0990, 0x0247, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_9]
    {0x0990, 0x0020, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_10]
    {0x0990, 0x0031, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_0]
    {0x0990, 0x00B6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_1]
    {0x0990, 0xFF7C, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_2]
    {0x0990, 0xFFD6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_3]
    {0x0990, 0x0047, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_4]
    {0x0990, 0x0019, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_5]
    {0x0990, 0xFFA7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_6]
    {0x0990, 0x0037, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_7]
    {0x0990, 0x00D9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_8]
    {0x0990, 0xFEF7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_9]
    {0x0990, 0x0015, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48DA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_10]
    {0x0990, 0xFFFE, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CBA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CBC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CBE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_9]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_10]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CCA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CCC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CCE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_9]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CDA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_10]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84A, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_R_L]
    {0x0990, 0x0070, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84B, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_G_L]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84C, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_B_L]
    {0x0990, 0x0087, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_R_R]
    {0x0990, 0x007F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84E, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_G_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_B_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4A, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_R_L]
    {0x0990, 0x0070, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4B, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_G_L]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4C, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_B_L]
    {0x0990, 0x0087, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_R_R]
    {0x0990, 0x007F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4E, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_G_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_B_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0xE86F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_LL_START_SATURATION]
    {0x0990, 0x0058, WORD_LEN, 0}, 	// MCU_DATA_0

    //[Gamma]
    {0x098E, 0xBC0B, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC0C, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
    {0x0990, 0x000A, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC0D, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
    {0x0990, 0x001D, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC0E, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
    {0x0990, 0x0037, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC0F, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
    {0x0990, 0x0058, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC10, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
    {0x0990, 0x0071, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC11, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
    {0x0990, 0x0086, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC12, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
    {0x0990, 0x0098, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC13, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
    {0x0990, 0x00A7, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC14, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
    {0x0990, 0x00B5, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC15, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
    {0x0990, 0x00C0, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC16, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
    {0x0990, 0x00CB, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC17, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
    {0x0990, 0x00D4, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC18, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
    {0x0990, 0x00DD, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC19, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
    {0x0990, 0x00E4, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC1A, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
    {0x0990, 0x00EC, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC1B, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
    {0x0990, 0x00F3, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC1C, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
    {0x0990, 0x00F9, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC1D, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC1E, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC1F, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_1]
    {0x0990, 0x000A, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC20, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_2]
    {0x0990, 0x001D, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC21, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_3]
    {0x0990, 0x0037, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC22, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_4]
    {0x0990, 0x0058, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC23, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_5]
    {0x0990, 0x0071, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC24, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_6]
    {0x0990, 0x0086, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC25, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_7]
    {0x0990, 0x0098, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC26, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_8]
    {0x0990, 0x00A7, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC27, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_9]
    {0x0990, 0x00B5, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC28, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_10]
    {0x0990, 0x00C0, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC29, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_11]
    {0x0990, 0x00CB, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC2A, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_12]
    {0x0990, 0x00D4, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC2B, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_13]
    {0x0990, 0x00DD, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC2C, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_14]
    {0x0990, 0x00E4, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC2D, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_15]
    {0x0990, 0x00EC, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC2E, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_16]
    {0x0990, 0x00F3, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC2F, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_17]
    {0x0990, 0x00F9, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC30, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NEUTRAL_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC31, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC32, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_1]
    {0x0990, 0x000A, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC33, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_2]
    {0x0990, 0x001D, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC34, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_3]
    {0x0990, 0x0037, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC35, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_4]
    {0x0990, 0x0058, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC36, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_5]
    {0x0990, 0x0071, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC37, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_6]
    {0x0990, 0x0086, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC38, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_7]
    {0x0990, 0x0098, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC39, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_8]
    {0x0990, 0x00A7, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC3A, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_9]
    {0x0990, 0x00B5, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC3B, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_10]
    {0x0990, 0x00C0, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC3C, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_11]
    {0x0990, 0x00CB, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC3D, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_12]
    {0x0990, 0x00D4, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC3E, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_13]
    {0x0990, 0x00DD, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC3F, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_14]
    {0x0990, 0x00E4, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC40, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_15]
    {0x0990, 0x00EC, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC41, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_16]
    {0x0990, 0x00F3, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC42, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_17]
    {0x0990, 0x00F9, WORD_LEN, 0},   // MCU_DATA_0
    {0x098E, 0xBC43, WORD_LEN, 0},   // MCU_ADDRESS [LL_GAMMA_NRCURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0},   // MCU_DATA_0

    //TIMING 40MHz-48mhz max frame rate 24.4FPS

    /*
      * Set Preview Size as (2048/2=1024)(W)x(1536/2=768)(H)
      */
    {0x098E, 0x6800, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 1024,    WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x6802, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 768,    WORD_LEN, 0},  // MCU_DATA_0

    {0x098E, 0xE88E, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x68A0, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x082E, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4802, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4804, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4806, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x060D, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4808, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x080D, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x480A, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0111, WORD_LEN, 0},  // MCU_DATA_0

    /*
      * For orientation setting of preview
      * bit1:bit0 = 0:0 default orientation
      * bit1:bit0 = 0:1 vertical flip
      * bit1:bit0 = 1:0 horizontal mirror
      * bit1:bit0 = 1:1 vertical flip & horizontal mirror
      */
#if defined(CONFIG_MACH_BLADE)
    {0x098E, 0x480C, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x046F, WORD_LEN, 0},  // MCU_DATA_0
#elif defined(CONFIG_MACH_RAISE)
    {0x098E, 0x480C, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x046C, WORD_LEN, 0},  // MCU_DATA_0
#else
    {0x098E, 0x480C, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x046F, WORD_LEN, 0},  // MCU_DATA_0
#endif

    {0x098E, 0x480F, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x00CC, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4811, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0381, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4813, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x024F, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x481D, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x035B, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x481F, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x05D0, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4825, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x07F2, WORD_LEN, 0},  // MCU_DATA_0

    /* 
      * Set Snapshot Size as Full Resolution, 2048(W)x1536(H)
      */
    {0x098E, 0x6C00, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 2048,   WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x6C02, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 1536,   WORD_LEN, 0},  // MCU_DATA_0

    {0x098E, 0xEC8E, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x6CA0, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x082E, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x484A, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0004, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x484C, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0004, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x484E, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x060B, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4850, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x080B, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4852, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0111, WORD_LEN, 0},  // MCU_DATA_0

    /*
      * For orientation setting of snapshot
      * bit1:bit0 = 0:0 default orientation
      * bit1:bit0 = 0:1 vertical flip
      * bit1:bit0 = 1:0 horizontal mirror
      * bit1:bit0 = 1:1 vertical flip & horizontal mirror
      */
#if defined(CONFIG_MACH_BLADE)
    {0x098E, 0x4854, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0027, WORD_LEN, 0},  // MCU_DATA_0
#elif defined(CONFIG_MACH_RAISE)
    {0x098E, 0x4854, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0024, WORD_LEN, 0},  // MCU_DATA_0
#else
    {0x098E, 0x4854, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0027, WORD_LEN, 0},  // MCU_DATA_0
#endif
    {0x098E, 0x4857, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x008C, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4859, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x01F1, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x485B, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x00FF, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4865, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x065D, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4867, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0378, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x486D, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0F69, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC8A5, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x002A, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC8A6, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x002D, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC8A7, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0032, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC8A8, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0035, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC844, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x00D2, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC92F, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC845, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x00AF, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC92D, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC88C, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x006C, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC930, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC88D, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x005A, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xC92E, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0000, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xA002, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0010, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xA009, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0002, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xA00A, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0003, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0xA00C, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x000A, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x4846, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0014, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x68AA, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0218, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x488E, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0014, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x6CAA, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x008A, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0006, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0005, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x683B, WORD_LEN, 0},  // MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_AE_MIN_FDZONE]
    {0x0990, 0x0001, WORD_LEN, 0},  // MCU_DATA_0 zhang.shengjie changed 
    {0x098E, 0x682D, WORD_LEN, 0},  // MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_TARGET_FDZONE]
    {0x0990, 0x0008, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x6815, WORD_LEN, 0},  // MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_50HZ]
    {0x0990, 0x0007, WORD_LEN, 0},  // MCU_DATA_0
    {0x098E, 0x6817, WORD_LEN, 0},  // MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_60HZ]
    {0x0990, 0x0008, WORD_LEN, 0},  // MCU_DATA_0
    {0x001A, 0x0218, WORD_LEN, 0},  // RESET_AND_MISC_CONTROL
    {0x3088, 0xFFFF, WORD_LEN, 0},  // RESERVED_CORE_3088
    {0x3172, 0x0003, WORD_LEN, 0},  // RESERVED_CORE_3172
    {0x316C, 0x0406, WORD_LEN, 0},  // RESERVED_CORE_316C
    {0x30FE, 0x4A38, WORD_LEN, 0},  // RESERVED_CORE_30FE
    {0x3100, 0x4738, WORD_LEN, 0},  // RESERVED_CORE_3100
    {0x3126, 0x00FF, WORD_LEN, 0},  // RESERVED_CORE_3126

    {0x098E, 0x4403, WORD_LEN, 0},    // MCU_ADDRESS [AFM_ALGO]
    {0x0990, 0x8001, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x440B, WORD_LEN, 0},    // MCU_ADDRESS [AFM_POS_MIN]
    {0x0990, 0x0032, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x440D, WORD_LEN, 0},    // MCU_ADDRESS [AFM_POS_MAX]
    {0x0990, 0x03B6, WORD_LEN, 0},  // MCU_DATA_0
    
    //[Refresh & refresh mode]
    {0x098E, 0x8400, WORD_LEN, 0},  // MCU_ADDRESS
    {0x0990, 0x0006, WORD_LEN, 10}, // MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const wb_cloudy_tbl[] = {
    {0x098E, 0x48B0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_0]
    {0x0990, 0x01F9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_1]
    {0x0990, 0xFF27, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_2]
    {0x0990, 0xFFFA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_3]
    {0x0990, 0xFFFD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_4]
    {0x0990, 0x01A6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_5]
    {0x0990, 0xFF76, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_6]
    {0x0990, 0x0023, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_7]
    {0x0990, 0xFF6A, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_8]
    {0x0990, 0x018D, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC02, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_MODE]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x2C03, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_ALGO]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x683F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3C, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_R_RATIO_PRE_AWB]
    {0x0990, 0x0035, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3D, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_B_RATIO_PRE_AWB]
    {0x0990, 0x006E, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0x6815, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_50HZ]
    {0x0990, 0x0007, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x6817, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_60HZ]
    {0x0990, 0x0008, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x682D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_TARGET_FDZONE]
    {0x0990, 0x0006, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const wb_daylight_tbl[] = {
    {0x098E, 0x48B0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_0]
    {0x0990, 0x01F9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_1]
    {0x0990, 0xFF27, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_2]
    {0x0990, 0xFFFA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_3]
    {0x0990, 0xFFFD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_4]
    {0x0990, 0x01A6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_5]
    {0x0990, 0xFF76, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_6]
    {0x0990, 0x0023, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_7]
    {0x0990, 0xFF6A, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_8]
    {0x0990, 0x018D, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC02, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_MODE]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x2C03, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_ALGO]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x683F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3C, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_R_RATIO_PRE_AWB]
    {0x0990, 0x0037, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3D, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_B_RATIO_PRE_AWB]
    {0x0990, 0x0065, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x6815, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_50HZ]
    {0x0990, 0x0007, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x6817, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_60HZ]
    {0x0990, 0x0008, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x682D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_TARGET_FDZONE]
    {0x0990, 0x0006, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const wb_flourescant_tbl[] = {
    {0x098E, 0x48B0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_0]
    {0x0990, 0x01F9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_1]
    {0x0990, 0xFF27, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_2]
    {0x0990, 0xFFFA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_3]
    {0x0990, 0xFFFD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_4]
    {0x0990, 0x01A6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_5]
    {0x0990, 0xFF76, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_6]
    {0x0990, 0x0023, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_7]
    {0x0990, 0xFF6A, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_8]
    {0x0990, 0x018D, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC02, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_MODE]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x2C03, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_ALGO]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x683F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3C, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_R_RATIO_PRE_AWB]
    {0x0990, 0x0048, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3D, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_B_RATIO_PRE_AWB]
    {0x0990, 0x004A, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0x6815, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_50HZ]
    {0x0990, 0x0007, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x6817, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_60HZ]
    {0x0990, 0x0008, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x682D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_TARGET_FDZONE]
    {0x0990, 0x0006, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const wb_incandescent_tbl[] = {
    {0x098E, 0x48B0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_0]
    {0x0990, 0x0180, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_1]
    {0x0990, 0xFF7A, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_2]
    {0x0990, 0x0018, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_3]
    {0x0990, 0xFFCA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_4]
    {0x0990, 0x017C, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_5]
    {0x0990, 0xFFCC, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_6]
    {0x0990, 0x000C, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_7]
    {0x0990, 0xFF1F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_8]
    {0x0990, 0x01E8, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC02, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_MODE]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x2C03, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_ALGO]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x683F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3C, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_R_RATIO_PRE_AWB]
    {0x0990, 0x0066, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xAC3D, WORD_LEN, 0}, 	// MCU_ADDRESS [AWB_B_RATIO_PRE_AWB]
    {0x0990, 0x0034, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0x6815, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_50HZ]
    {0x0990, 0x0007, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x6817, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_FD_MAX_FDZONE_60HZ]
    {0x0990, 0x0008, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x682D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AE_TRACK_TARGET_FDZONE]
    {0x0990, 0x0006, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const wb_auto_tbl[] = {
     /*
      * Otherwise when WB return to auto mode, we can not see the effect actually
      */    
    {0x098E, 0xAC02, WORD_LEN, 0},     // MCU_ADDRESS [AWB_MODE]
    {0x0990, 0x000A, WORD_LEN, 0},     // MCU_DATA_0
    {0x098E, 0x2C03, WORD_LEN, 0},     // MCU_ADDRESS                              
    {0x0990, 0x01ff, WORD_LEN, 0},     // MCU_DATA_0                             
    {0x098E, 0x683F, WORD_LEN, 0},     // MCU_ADDRESS [PRI_A_CONFIG_AWB_ALGO_RUN]
    {0x0990, 0x01fF, WORD_LEN, 0},     // MCU_DATA_0

    {0x098E, 0x48B0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_0]
    {0x0990, 0x0143, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_1]
    {0x0990, 0xFFAB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_2]
    {0x0990, 0x0024, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_3]
    {0x0990, 0xFFC6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48B8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_4]
    {0x0990, 0x0167, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_5]
    {0x0990, 0xFFE5, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_6]
    {0x0990, 0x000E, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48BE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_7]
    {0x0990, 0xFEBD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_8]
    {0x0990, 0x0247, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_9]
    {0x0990, 0x0020, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_L_10]
    {0x0990, 0x0031, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_0]
    {0x0990, 0x00B6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48C8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_1]
    {0x0990, 0xFF7C, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_2]
    {0x0990, 0xFFD6, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_3]
    {0x0990, 0x0047, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48CE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_4]
    {0x0990, 0x0019, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_5]
    {0x0990, 0xFFA7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_6]
    {0x0990, 0x0037, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_7]
    {0x0990, 0x00D9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_8]
    {0x0990, 0xFEF7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48D8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_9]
    {0x0990, 0x0015, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x48DA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM1_AWB_CCM_RL_10]
    {0x0990, 0xFFFE, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CB8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CBA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CBC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CBE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_9]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_L_10]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CC8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_1]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CCA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_2]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CCC, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_3]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CCE, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_4]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD0, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_5]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD2, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_6]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD4, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_7]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD6, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_8]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CD8, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_9]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x4CDA, WORD_LEN, 0}, 	// MCU_ADDRESS [CAM2_AWB_CCM_RL_10]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84A, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_R_L]
    {0x0990, 0x0070, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84B, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_G_L]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84C, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_B_L]
    {0x0990, 0x0087, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_R_R]
    {0x0990, 0x007F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84E, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_G_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xE84F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_AWB_K_B_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4A, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_R_L]
    {0x0990, 0x0070, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4B, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_G_L]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4C, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_B_L]
    {0x0990, 0x0087, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4D, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_R_R]
    {0x0990, 0x007F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4E, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_G_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xEC4F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_B_CONFIG_AWB_K_B_R]
    {0x0990, 0x0080, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0xE86F, WORD_LEN, 0}, 	// MCU_ADDRESS [PRI_A_CONFIG_LL_START_SATURATION]
    {0x0990, 0x0058, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0006, WORD_LEN, 0}, 	// MCU_DATA_0
};

/* Auto focus Setup */
static struct mt9t11x_i2c_reg_conf const af_tbl[] = {
    {0x0604, 0x0101, WORD_LEN, 0},    // GPIO_DATA
    {0x0606, 0x0001, WORD_LEN, 0},    // GPIO_DIR
    {0x098E, 0x4403, WORD_LEN, 0},    // MCU_ADDRESS [AFM_ALGO]
    {0x0990, 0x8001, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x440B, WORD_LEN, 0},    // MCU_ADDRESS [AFM_POS_MIN]
    {0x0990, 0x0032, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x440D, WORD_LEN, 0},    // MCU_ADDRESS [AFM_POS_MAX]
    {0x0990, 0x03B6, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0},    // MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0006, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x3020, WORD_LEN, 0},    // MCU_ADDRESS [AF_ZONE_WEIGHTS_HI]
    {0x0990, 0x557D, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x3022, WORD_LEN, 0},    // MCU_ADDRESS [AF_ZONE_WEIGHTS_LO]
    {0x0990, 0x7D55, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0xB024, WORD_LEN, 0},    // MCU_ADDRESS [AF_BEST_POSITION]
    {0x0990, 0x0000, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0x3003, WORD_LEN, 0},    // MCU_ADDRESS [AF_ALGO]
    {0x0990, 0x0002, WORD_LEN, 0},    // MCU_DATA_0
    {0x098E, 0xB019, WORD_LEN, 0},    // MCU_ADDRESS [AF_ALGO]
    {0x0990, 0x0001, WORD_LEN, 0},    // MCU_DATA_0
};

/* Contrast Setup */
static struct mt9t11x_i2c_reg_conf const contrast_tbl_0[] = {
    {0x098E, 0xBC08, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_SELECT]
    {0x0990, 0x0001, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
    {0x0990, 0x0030, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
    {0x0990, 0x0040, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0E, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
    {0x0990, 0x0054, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0F, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
    {0x0990, 0x006F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC10, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
    {0x0990, 0x0083, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC11, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
    {0x0990, 0x0092, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC12, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
    {0x0990, 0x00A0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC13, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
    {0x0990, 0x00AC, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC14, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
    {0x0990, 0x00B7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC15, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
    {0x0990, 0x00C1, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC16, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
    {0x0990, 0x00CB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC17, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
    {0x0990, 0x00D3, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC18, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
    {0x0990, 0x00DC, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC19, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
    {0x0990, 0x00E3, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1A, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
    {0x0990, 0x00EB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
    {0x0990, 0x00F2, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
    {0x0990, 0x00F9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const contrast_tbl_1[] = {
    {0x098E, 0xBC08, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_SELECT]
    {0x0990, 0x0001, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
    {0x0990, 0x000D, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
    {0x0990, 0x0024, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0E, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
    {0x0990, 0x0042, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0F, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
    {0x0990, 0x0064, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC10, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
    {0x0990, 0x007E, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC11, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
    {0x0990, 0x0092, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC12, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
    {0x0990, 0x00A3, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC13, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
    {0x0990, 0x00B1, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC14, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
    {0x0990, 0x00BC, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC15, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
    {0x0990, 0x00C7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC16, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
    {0x0990, 0x00D0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC17, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
    {0x0990, 0x00D9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC18, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
    {0x0990, 0x00E0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC19, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
    {0x0990, 0x00E7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1A, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
    {0x0990, 0x00EE, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
    {0x0990, 0x00F4, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
    {0x0990, 0x00FA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const contrast_tbl_2[] = {
    {0x098E, 0xBC08, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_SELECT]
    {0x0990, 0x0001, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0xBC0B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
    {0x0990, 0x000A, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
    {0x0990, 0x001D, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0E, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
    {0x0990, 0x0037, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0F, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
    {0x0990, 0x0058, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC10, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
    {0x0990, 0x0071, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC11, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
    {0x0990, 0x0086, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC12, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
    {0x0990, 0x0098, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC13, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
    {0x0990, 0x00A7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC14, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
    {0x0990, 0x00B5, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC15, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
    {0x0990, 0x00C0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC16, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
    {0x0990, 0x00CB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC17, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
    {0x0990, 0x00D4, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC18, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
    {0x0990, 0x00DD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC19, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
    {0x0990, 0x00E4, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1A, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
    {0x0990, 0x00EC, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
    {0x0990, 0x00F3, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
    {0x0990, 0x00F9, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const contrast_tbl_3[] = {
    {0x098E, 0xBC08, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_SELECT]
    {0x0990, 0x0001, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0xBC0B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
    {0x0990, 0x0008, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
    {0x0990, 0x0017, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0E, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
    {0x0990, 0x002E, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0F, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
    {0x0990, 0x004F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC10, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
    {0x0990, 0x006C, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC11, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
    {0x0990, 0x0088, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC12, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
    {0x0990, 0x009F, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC13, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
    {0x0990, 0x00B0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC14, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
    {0x0990, 0x00BE, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC15, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
    {0x0990, 0x00CA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC16, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
    {0x0990, 0x00D4, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC17, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
    {0x0990, 0x00DC, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC18, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
    {0x0990, 0x00E4, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC19, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
    {0x0990, 0x00EA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1A, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
    {0x0990, 0x00F0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
    {0x0990, 0x00F5, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
    {0x0990, 0x00FA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const contrast_tbl_4[] = {
    {0x098E, 0xBC08, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_SELECT]
    {0x0990, 0x0001, WORD_LEN, 0}, 	// MCU_DATA_0

    {0x098E, 0xBC0B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_0]
    {0x0990, 0x0000, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_1]
    {0x0990, 0x0003, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_2]
    {0x0990, 0x0009, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0E, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_3]
    {0x0990, 0x0015, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC0F, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_4]
    {0x0990, 0x002B, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC10, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_5]
    {0x0990, 0x0042, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC11, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_6]
    {0x0990, 0x005B, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC12, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_7]
    {0x0990, 0x0078, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC13, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_8]
    {0x0990, 0x0095, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC14, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_9]
    {0x0990, 0x00AB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC15, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_10]
    {0x0990, 0x00BD, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC16, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_11]
    {0x0990, 0x00CB, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC17, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_12]
    {0x0990, 0x00D7, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC18, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_13]
    {0x0990, 0x00E0, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC19, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_14]
    {0x0990, 0x00E8, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1A, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_15]
    {0x0990, 0x00EF, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1B, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_16]
    {0x0990, 0x00F5, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1C, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_17]
    {0x0990, 0x00FA, WORD_LEN, 0}, 	// MCU_DATA_0
    {0x098E, 0xBC1D, WORD_LEN, 0}, 	// MCU_ADDRESS [LL_GAMMA_CONTRAST_CURVE_18]
    {0x0990, 0x00FF, WORD_LEN, 0}, 	// MCU_DATA_0


    {0x098E, 0x8400, WORD_LEN, 0}, 	// MCU_ADDRESS [SEQ_CMD]
    {0x0990, 0x0005, WORD_LEN, 0}, 	// MCU_DATA_0
};

static struct mt9t11x_i2c_reg_conf const *contrast_tbl[] = {
    contrast_tbl_0,
    contrast_tbl_1,
    contrast_tbl_2,
    contrast_tbl_3,
    contrast_tbl_4,
};

static uint16_t const contrast_tbl_sz[] = {
    ARRAY_SIZE(contrast_tbl_0),
    ARRAY_SIZE(contrast_tbl_1),
    ARRAY_SIZE(contrast_tbl_2),
    ARRAY_SIZE(contrast_tbl_3),
    ARRAY_SIZE(contrast_tbl_4),
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_0[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x0001, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_1[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x000A, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_2[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x0014, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_3[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x0020, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_4[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x0030, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_5[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x0060, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const brightness_tbl_6[] = {
    {0x098E, 0xE81F, WORD_LEN, 0},
    {0x0990, 0x00cf, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const *brightness_tbl[] = {
    brightness_tbl_0,
    brightness_tbl_1,
    brightness_tbl_2,
    brightness_tbl_3,
    brightness_tbl_4,
    brightness_tbl_5,
    brightness_tbl_6,
};

static uint16_t const brightness_tbl_sz[] = {
    ARRAY_SIZE(brightness_tbl_0),
    ARRAY_SIZE(brightness_tbl_1),
    ARRAY_SIZE(brightness_tbl_2),
    ARRAY_SIZE(brightness_tbl_3),
    ARRAY_SIZE(brightness_tbl_4),
    ARRAY_SIZE(brightness_tbl_5),
    ARRAY_SIZE(brightness_tbl_6),
};

static struct mt9t11x_i2c_reg_conf const saturation_tbl_0[] = {
    {0x098E, 0xE86F, WORD_LEN, 0},
    {0x0990, 0x0000, WORD_LEN, 0},
    {0x098E, 0xE870, WORD_LEN, 0},
    {0x0990, 0x0000, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const saturation_tbl_1[] = {
    {0x098E, 0xE86F, WORD_LEN, 0},
    {0x0990, 0x0040, WORD_LEN, 0},
    {0x098E, 0xE870, WORD_LEN, 0},
    {0x0990, 0x0040, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const saturation_tbl_2[] = {
    {0x098E, 0xE86F, WORD_LEN, 0},
    {0x0990, 0x0080, WORD_LEN, 0},
    {0x098E, 0xE870, WORD_LEN, 0},
    {0x0990, 0x0080, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const saturation_tbl_3[] = {
    {0x098E, 0xE86F, WORD_LEN, 0},
    {0x0990, 0x00C0, WORD_LEN, 0},
    {0x098E, 0xE870, WORD_LEN, 0},
    {0x0990, 0x00c0, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const saturation_tbl_4[] = {
    {0x098E, 0xE86F, WORD_LEN, 0},
    {0x0990, 0x00FF, WORD_LEN, 0},
    {0x098E, 0xE870, WORD_LEN, 0},
    {0x0990, 0x00ff, WORD_LEN, 0},
};

static struct mt9t11x_i2c_reg_conf const *saturation_tbl[] = {
    saturation_tbl_0,
    saturation_tbl_1,
    saturation_tbl_2,
    saturation_tbl_3,
    saturation_tbl_4,
};

static uint16_t const saturation_tbl_sz[] = {
    ARRAY_SIZE(saturation_tbl_0),
    ARRAY_SIZE(saturation_tbl_1),
    ARRAY_SIZE(saturation_tbl_2),
    ARRAY_SIZE(saturation_tbl_3),
    ARRAY_SIZE(saturation_tbl_4),
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_0[] = {
    {0x326C, 0x110A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0001, WORD_LEN, 0},
    {0x3210, 0x04B8, WORD_LEN, 0},        
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_1[] = {
    {0x326C, 0x120A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0002, WORD_LEN, 0},
    {0x3210, 0x04B8, WORD_LEN, 0},        
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_2[] = {
    {0x326C, 0x140A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0004, WORD_LEN, 0},
    {0x3210, 0x04B8, WORD_LEN, 0},        
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_3[] = {
    {0x326C, 0x160A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0006, WORD_LEN, 0},
    {0x3210, 0x04B8, WORD_LEN, 0},        
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_4[] = {
    {0x326C, 0x170A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0007, WORD_LEN, 0},
    {0x3210, 0x04B8, WORD_LEN, 0},    
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_5[] = {
    //{0x326C, 0x110A, WORD_LEN, 0},
    {0x326C, 0x080A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0000, WORD_LEN, 0},
    {0x098E, 0xC918, WORD_LEN, 0},
    {0x0990, 0x000A, WORD_LEN, 0},    
    {0x3210, 0x01B8, WORD_LEN, 0},        
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_6[] = {
    //{0x326C, 0x120A, WORD_LEN, 0},
    {0x326C, 0x0A0A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0002, WORD_LEN, 0},
    {0x098E, 0xC918, WORD_LEN, 0},
    {0x0990, 0x000A, WORD_LEN, 0},    
    {0x3210, 0x01B8, WORD_LEN, 0},        
};


static struct mt9t11x_i2c_reg_conf const sharpness_tbl_7[] = {
    {0x098E, 0xC917, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_1]
    {0x0990, 0x0002, WORD_LEN, 0},     // MCU_DATA_0
    {0x098E, 0xC918, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_2]
    {0x0990, 0x0000, WORD_LEN, 0},     // MCU_DATA_0                
    {0x326C, 0x1C02, WORD_LEN, 0},     // APERTURE_PARAMETERS_2D
};

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_8[] = {
    {0x098E, 0xC917, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_1]
    {0x0990, 0x0004, WORD_LEN, 0},     // MCU_DATA_0
    {0x098E, 0xC918, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_2]
    {0x0990, 0x0002, WORD_LEN, 0},     // MCU_DATA_0                   
    {0x326C, 0x1C02, WORD_LEN, 0},     // APERTURE_PARAMETERS_2D
};    

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_9[] = {
    {0x098E, 0xC917, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_1]
    {0x0990, 0x0006, WORD_LEN, 0},     // MCU_DATA_0
    {0x098E, 0xC918, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_2]
    {0x0990, 0x0002, WORD_LEN, 0},     // MCU_DATA_0             
    {0x326C, 0x1C02, WORD_LEN, 0},     // APERTURE_PARAMETERS_2D
};    

static struct mt9t11x_i2c_reg_conf const sharpness_tbl_10[] = {
    //{0x326C, 0x110A, WORD_LEN, 0},
    {0x326C, 0x080A, WORD_LEN, 0},
    {0x098E, 0xC917, WORD_LEN, 0},
    {0x0990, 0x0000, WORD_LEN, 0},
    {0x098E, 0xC918, WORD_LEN, 0},     // MCU_ADDRESS [CAM1_LL_LL_START_2]
    {0x0990, 0x000A, WORD_LEN, 0},     // MCU_DATA_0   
    {0x3210, 0x01B8, WORD_LEN, 0},        
};
static struct mt9t11x_i2c_reg_conf const *sharpness_tbl[] = {
    sharpness_tbl_0,
    sharpness_tbl_1,
    sharpness_tbl_2,
    sharpness_tbl_3,
    sharpness_tbl_4,
    sharpness_tbl_5,
    sharpness_tbl_6,
    sharpness_tbl_7,
    sharpness_tbl_8,
    sharpness_tbl_9,    
    sharpness_tbl_10,
};

static uint16_t const sharpness_tbl_sz[] = {
    ARRAY_SIZE(sharpness_tbl_0),
    ARRAY_SIZE(sharpness_tbl_1),
    ARRAY_SIZE(sharpness_tbl_2),
    ARRAY_SIZE(sharpness_tbl_3),
    ARRAY_SIZE(sharpness_tbl_4),
    ARRAY_SIZE(sharpness_tbl_5),
    ARRAY_SIZE(sharpness_tbl_6),
    ARRAY_SIZE(sharpness_tbl_7),
    ARRAY_SIZE(sharpness_tbl_8),
    ARRAY_SIZE(sharpness_tbl_9),
    ARRAY_SIZE(sharpness_tbl_10),    
};

static struct mt9t11x_i2c_reg_conf const exposure_tbl_0[] = {
    {0x337E, 0xC000, WORD_LEN, 0},  // Y_RGB_OFFSET
};

static struct mt9t11x_i2c_reg_conf const exposure_tbl_1[] = {
    {0x337E, 0xE000, WORD_LEN, 0},  // Y_RGB_OFFSET
};

static struct mt9t11x_i2c_reg_conf const exposure_tbl_2[] = {
    {0x337E, 0x0000, WORD_LEN, 0},  // Y_RGB_OFFSET
};

static struct mt9t11x_i2c_reg_conf const exposure_tbl_3[] = {
    {0x337E, 0x1800, WORD_LEN, 0},  // Y_RGB_OFFSET
};

static struct mt9t11x_i2c_reg_conf const exposure_tbl_4[] = {
    {0x337E, 0x3000, WORD_LEN, 0},  // Y_RGB_OFFSET
};

static struct mt9t11x_i2c_reg_conf const *exposure_tbl[] = {
    exposure_tbl_0,
    exposure_tbl_1,
    exposure_tbl_2,
    exposure_tbl_3,
    exposure_tbl_4,
};

static uint16_t const exposure_tbl_sz[] = {
    ARRAY_SIZE(exposure_tbl_0),
    ARRAY_SIZE(exposure_tbl_1),
    ARRAY_SIZE(exposure_tbl_2),
    ARRAY_SIZE(exposure_tbl_3),
    ARRAY_SIZE(exposure_tbl_4),
};

static struct mt9t11x_i2c_reg_conf const lens_for_outdoor_tbl[] ={
    //[85% with white paper in sunlight]
    {0x3640, 0x08B0, WORD_LEN, 0}, 	// P_G1_P0Q0
    {0x3642, 0x9F8E, WORD_LEN, 0}, 	// P_G1_P0Q1
    {0x3644, 0x6A71, WORD_LEN, 0}, 	// P_G1_P0Q2
    {0x3646, 0x288D, WORD_LEN, 0}, 	// P_G1_P0Q3
    {0x3648, 0x9212, WORD_LEN, 0}, 	// P_G1_P0Q4
    {0x364A, 0x02B0, WORD_LEN, 0}, 	// P_R_P0Q0
    {0x364C, 0xD24E, WORD_LEN, 0}, 	// P_R_P0Q1
    {0x364E, 0x1432, WORD_LEN, 0}, 	// P_R_P0Q2
    {0x3650, 0x3D8E, WORD_LEN, 0}, 	// P_R_P0Q3
    {0x3652, 0xA372, WORD_LEN, 0}, 	// P_R_P0Q4
    {0x3654, 0x0690, WORD_LEN, 0}, 	// P_B_P0Q0
    {0x3656, 0xD66D, WORD_LEN, 0}, 	// P_B_P0Q1
    {0x3658, 0x3C51, WORD_LEN, 0}, 	// P_B_P0Q2
    {0x365A, 0x7FAD, WORD_LEN, 0}, 	// P_B_P0Q3
    {0x365C, 0xDE51, WORD_LEN, 0}, 	// P_B_P0Q4
    {0x365E, 0x05D0, WORD_LEN, 0}, 	// P_G2_P0Q0
    {0x3660, 0xA5AE, WORD_LEN, 0}, 	// P_G2_P0Q1
    {0x3662, 0x6B91, WORD_LEN, 0}, 	// P_G2_P0Q2
    {0x3664, 0x664D, WORD_LEN, 0}, 	// P_G2_P0Q3
    {0x3666, 0x9BB2, WORD_LEN, 0}, 	// P_G2_P0Q4
    {0x3680, 0x382E, WORD_LEN, 0}, 	// P_G1_P1Q0
    {0x3682, 0xAB8F, WORD_LEN, 0}, 	// P_G1_P1Q1
    {0x3684, 0x6690, WORD_LEN, 0}, 	// P_G1_P1Q2
    {0x3686, 0x1472, WORD_LEN, 0}, 	// P_G1_P1Q3
    {0x3688, 0xD712, WORD_LEN, 0}, 	// P_G1_P1Q4
    {0x368A, 0x5AEE, WORD_LEN, 0}, 	// P_R_P1Q0
    {0x368C, 0xA0CF, WORD_LEN, 0}, 	// P_R_P1Q1
    {0x368E, 0x18B1, WORD_LEN, 0}, 	// P_R_P1Q2
    {0x3690, 0x0C72, WORD_LEN, 0}, 	// P_R_P1Q3
    {0x3692, 0xF1B2, WORD_LEN, 0}, 	// P_R_P1Q4
    {0x3694, 0xFBC8, WORD_LEN, 0}, 	// P_B_P1Q0
    {0x3696, 0xA36E, WORD_LEN, 0}, 	// P_B_P1Q1
    {0x3698, 0x082F, WORD_LEN, 0}, 	// P_B_P1Q2
    {0x369A, 0x2D31, WORD_LEN, 0}, 	// P_B_P1Q3
    {0x369C, 0xDBAF, WORD_LEN, 0}, 	// P_B_P1Q4
    {0x369E, 0xC069, WORD_LEN, 0}, 	// P_G2_P1Q0
    {0x36A0, 0xB20E, WORD_LEN, 0}, 	// P_G2_P1Q1
    {0x36A2, 0x6A2F, WORD_LEN, 0}, 	// P_G2_P1Q2
    {0x36A4, 0x5C31, WORD_LEN, 0}, 	// P_G2_P1Q3
    {0x36A6, 0xB791, WORD_LEN, 0}, 	// P_G2_P1Q4
    {0x36C0, 0x5532, WORD_LEN, 0}, 	// P_G1_P2Q0
    {0x36C2, 0xEC0E, WORD_LEN, 0}, 	// P_G1_P2Q1
    {0x36C4, 0xE9F4, WORD_LEN, 0}, 	// P_G1_P2Q2
    {0x36C6, 0xD472, WORD_LEN, 0}, 	// P_G1_P2Q3
    {0x36C8, 0x1955, WORD_LEN, 0}, 	// P_G1_P2Q4
    {0x36CA, 0x5E52, WORD_LEN, 0}, 	// P_R_P2Q0
    {0x36CC, 0x8B70, WORD_LEN, 0}, 	// P_R_P2Q1
    {0x36CE, 0xA1B4, WORD_LEN, 0}, 	// P_R_P2Q2
    {0x36D0, 0x298E, WORD_LEN, 0}, 	// P_R_P2Q3
    {0x36D2, 0xEB32, WORD_LEN, 0}, 	// P_R_P2Q4
    {0x36D4, 0x33F2, WORD_LEN, 0}, 	// P_B_P2Q0
    {0x36D6, 0x460D, WORD_LEN, 0}, 	// P_B_P2Q1
    {0x36D8, 0xDB14, WORD_LEN, 0}, 	// P_B_P2Q2
    {0x36DA, 0xBA73, WORD_LEN, 0}, 	// P_B_P2Q3
    {0x36DC, 0x3635, WORD_LEN, 0}, 	// P_B_P2Q4
    {0x36DE, 0x5412, WORD_LEN, 0}, 	// P_G2_P2Q0
    {0x36E0, 0x83AE, WORD_LEN, 0}, 	// P_G2_P2Q1
    {0x36E2, 0xEF34, WORD_LEN, 0}, 	// P_G2_P2Q2
    {0x36E4, 0x85B3, WORD_LEN, 0}, 	// P_G2_P2Q3
    {0x36E6, 0x2C15, WORD_LEN, 0}, 	// P_G2_P2Q4
    {0x3700, 0xB36F, WORD_LEN, 0}, 	// P_G1_P3Q0
    {0x3702, 0x3893, WORD_LEN, 0}, 	// P_G1_P3Q1
    {0x3704, 0xA6F5, WORD_LEN, 0}, 	// P_G1_P3Q2
    {0x3706, 0xF3D5, WORD_LEN, 0}, 	// P_G1_P3Q3
    {0x3708, 0x3177, WORD_LEN, 0}, 	// P_G1_P3Q4
    {0x370A, 0x0248, WORD_LEN, 0}, 	// P_R_P3Q0
    {0x370C, 0x5D33, WORD_LEN, 0}, 	// P_R_P3Q1
    {0x370E, 0xC215, WORD_LEN, 0}, 	// P_R_P3Q2
    {0x3710, 0x86D6, WORD_LEN, 0}, 	// P_R_P3Q3
    {0x3712, 0x3F57, WORD_LEN, 0}, 	// P_R_P3Q4
    {0x3714, 0x88EF, WORD_LEN, 0}, 	// P_B_P3Q0
    {0x3716, 0x14F3, WORD_LEN, 0}, 	// P_B_P3Q1
    {0x3718, 0xD8F4, WORD_LEN, 0}, 	// P_B_P3Q2
    {0x371A, 0xBAB5, WORD_LEN, 0}, 	// P_B_P3Q3
    {0x371C, 0x7216, WORD_LEN, 0}, 	// P_B_P3Q4
    {0x371E, 0xFEEF, WORD_LEN, 0}, 	// P_G2_P3Q0
    {0x3720, 0x3093, WORD_LEN, 0}, 	// P_G2_P3Q1
    {0x3722, 0xF094, WORD_LEN, 0}, 	// P_G2_P3Q2
    {0x3724, 0xE8D5, WORD_LEN, 0}, 	// P_G2_P3Q3
    {0x3726, 0x0ED7, WORD_LEN, 0}, 	// P_G2_P3Q4
    {0x3740, 0xA3F4, WORD_LEN, 0}, 	// P_G1_P4Q0
    {0x3742, 0x4F94, WORD_LEN, 0}, 	// P_G1_P4Q1
    {0x3744, 0x7D54, WORD_LEN, 0}, 	// P_G1_P4Q2
    {0x3746, 0xB8D5, WORD_LEN, 0}, 	// P_G1_P4Q3
    {0x3748, 0x3717, WORD_LEN, 0}, 	// P_G1_P4Q4
    {0x374A, 0xF5D3, WORD_LEN, 0}, 	// P_R_P4Q0
    {0x374C, 0x0DF5, WORD_LEN, 0}, 	// P_R_P4Q1
    {0x374E, 0xB555, WORD_LEN, 0}, 	// P_R_P4Q2
    {0x3750, 0xC656, WORD_LEN, 0}, 	// P_R_P4Q3
    {0x3752, 0x5EB8, WORD_LEN, 0}, 	// P_R_P4Q4
    {0x3754, 0x8394, WORD_LEN, 0}, 	// P_B_P4Q0
    {0x3756, 0x2534, WORD_LEN, 0}, 	// P_B_P4Q1
    {0x3758, 0x5BB5, WORD_LEN, 0}, 	// P_B_P4Q2
    {0x375A, 0x9D34, WORD_LEN, 0}, 	// P_B_P4Q3
    {0x375C, 0x61F5, WORD_LEN, 0}, 	// P_B_P4Q4
    {0x375E, 0xA014, WORD_LEN, 0}, 	// P_G2_P4Q0
    {0x3760, 0x3094, WORD_LEN, 0}, 	// P_G2_P4Q1
    {0x3762, 0x2195, WORD_LEN, 0}, 	// P_G2_P4Q2
    {0x3764, 0xC7D4, WORD_LEN, 0}, 	// P_G2_P4Q3
    {0x3766, 0x0437, WORD_LEN, 0}, 	// P_G2_P4Q4
    {0x3782, 0x0320, WORD_LEN, 0}, 	// CENTER_ROW
    {0x3784, 0x0380, WORD_LEN, 0}, 	// CENTER_COLUMN
    {0x3210, 0x01B8, WORD_LEN, 0}, 	// COLOR_PIPELINE_CONTROL
};

static struct mt9t11x_i2c_reg_conf const lens_for_indoor_tbl[] ={
    //[Lens Correction 90% 03/12/10 15:45:07]
    {0x364A, 0x0530, WORD_LEN, 0}, 	// P_R_P0Q0
    {0x364C, 0x9A4F, WORD_LEN, 0}, 	// P_R_P0Q1
    {0x364E, 0x4CF1, WORD_LEN, 0}, 	// P_R_P0Q2
    {0x3650, 0x49D1, WORD_LEN, 0}, 	// P_R_P0Q3
    {0x3652, 0xD372, WORD_LEN, 0}, 	// P_R_P0Q4
    {0x368A, 0x654C, WORD_LEN, 0}, 	// P_R_P1Q0
    {0x368C, 0x558F, WORD_LEN, 0}, 	// P_R_P1Q1
    {0x368E, 0x5D12, WORD_LEN, 0}, 	// P_R_P1Q2
    {0x3690, 0xFCEB, WORD_LEN, 0}, 	// P_R_P1Q3
    {0x3692, 0xE9D3, WORD_LEN, 0}, 	// P_R_P1Q4
    {0x36CA, 0x0ED2, WORD_LEN, 0}, 	// P_R_P2Q0
    {0x36CC, 0xD92E, WORD_LEN, 0}, 	// P_R_P2Q1
    {0x36CE, 0xA4B3, WORD_LEN, 0}, 	// P_R_P2Q2
    {0x36D0, 0xBA34, WORD_LEN, 0}, 	// P_R_P2Q3
    {0x36D2, 0x59B6, WORD_LEN, 0}, 	// P_R_P2Q4
    {0x370A, 0x3111, WORD_LEN, 0}, 	// P_R_P3Q0
    {0x370C, 0x85F4, WORD_LEN, 0}, 	// P_R_P3Q1
    {0x370E, 0x9B96, WORD_LEN, 0}, 	// P_R_P3Q2
    {0x3710, 0x44D5, WORD_LEN, 0}, 	// P_R_P3Q3
    {0x3712, 0x0FD7, WORD_LEN, 0}, 	// P_R_P3Q4
    {0x374A, 0xD652, WORD_LEN, 0}, 	// P_R_P4Q0
    {0x374C, 0xE253, WORD_LEN, 0}, 	// P_R_P4Q1
    {0x374E, 0xAFF6, WORD_LEN, 0}, 	// P_R_P4Q2
    {0x3750, 0x2DB7, WORD_LEN, 0}, 	// P_R_P4Q3
    {0x3752, 0x80B8, WORD_LEN, 0}, 	// P_R_P4Q4
    {0x3640, 0x0950, WORD_LEN, 0}, 	// P_G1_P0Q0
    {0x3642, 0x8EEF, WORD_LEN, 0}, 	// P_G1_P0Q1
    {0x3644, 0x4091, WORD_LEN, 0}, 	// P_G1_P0Q2
    {0x3646, 0x3871, WORD_LEN, 0}, 	// P_G1_P0Q3
    {0x3648, 0xCF92, WORD_LEN, 0}, 	// P_G1_P0Q4
    {0x3680, 0x042B, WORD_LEN, 0}, 	// P_G1_P1Q0
    {0x3682, 0x2C2F, WORD_LEN, 0}, 	// P_G1_P1Q1
    {0x3684, 0x5F32, WORD_LEN, 0}, 	// P_G1_P1Q2
    {0x3686, 0x566F, WORD_LEN, 0}, 	// P_G1_P1Q3
    {0x3688, 0x8314, WORD_LEN, 0}, 	// P_G1_P1Q4
    {0x36C0, 0x1532, WORD_LEN, 0}, 	// P_G1_P2Q0
    {0x36C2, 0xBE2F, WORD_LEN, 0}, 	// P_G1_P2Q1
    {0x36C4, 0xC733, WORD_LEN, 0}, 	// P_G1_P2Q2
    {0x36C6, 0xADD4, WORD_LEN, 0}, 	// P_G1_P2Q3
    {0x36C8, 0x5E16, WORD_LEN, 0}, 	// P_G1_P2Q4
    {0x3700, 0x16F1, WORD_LEN, 0}, 	// P_G1_P3Q0
    {0x3702, 0x8734, WORD_LEN, 0}, 	// P_G1_P3Q1
    {0x3704, 0xA056, WORD_LEN, 0}, 	// P_G1_P3Q2
    {0x3706, 0x2B15, WORD_LEN, 0}, 	// P_G1_P3Q3
    {0x3708, 0x2817, WORD_LEN, 0}, 	// P_G1_P3Q4
    {0x3740, 0x94B3, WORD_LEN, 0}, 	// P_G1_P4Q0
    {0x3742, 0xCBF3, WORD_LEN, 0}, 	// P_G1_P4Q1
    {0x3744, 0xA796, WORD_LEN, 0}, 	// P_G1_P4Q2
    {0x3746, 0x1877, WORD_LEN, 0}, 	// P_G1_P4Q3
    {0x3748, 0xD037, WORD_LEN, 0}, 	// P_G1_P4Q4
    {0x3654, 0x05D0, WORD_LEN, 0}, 	// P_B_P0Q0
    {0x3656, 0xD6EE, WORD_LEN, 0}, 	// P_B_P0Q1
    {0x3658, 0x1351, WORD_LEN, 0}, 	// P_B_P0Q2
    {0x365A, 0x2231, WORD_LEN, 0}, 	// P_B_P0Q3
    {0x365C, 0x9A52, WORD_LEN, 0}, 	// P_B_P0Q4
    {0x3694, 0x9DAE, WORD_LEN, 0}, 	// P_B_P1Q0
    {0x3696, 0x0370, WORD_LEN, 0}, 	// P_B_P1Q1
    {0x3698, 0x3DB2, WORD_LEN, 0}, 	// P_B_P1Q2
    {0x369A, 0x9051, WORD_LEN, 0}, 	// P_B_P1Q3
    {0x369C, 0xA433, WORD_LEN, 0}, 	// P_B_P1Q4
    {0x36D4, 0x0152, WORD_LEN, 0}, 	// P_B_P2Q0
    {0x36D6, 0xF9CE, WORD_LEN, 0}, 	// P_B_P2Q1
    {0x36D8, 0xB7B3, WORD_LEN, 0}, 	// P_B_P2Q2
    {0x36DA, 0xAFB4, WORD_LEN, 0}, 	// P_B_P2Q3
    {0x36DC, 0x54D6, WORD_LEN, 0}, 	// P_B_P2Q4
    {0x3714, 0x3611, WORD_LEN, 0}, 	// P_B_P3Q0
    {0x3716, 0x85F4, WORD_LEN, 0}, 	// P_B_P3Q1
    {0x3718, 0x87B6, WORD_LEN, 0}, 	// P_B_P3Q2
    {0x371A, 0x63F5, WORD_LEN, 0}, 	// P_B_P3Q3
    {0x371C, 0x6556, WORD_LEN, 0}, 	// P_B_P3Q4
    {0x3754, 0xE172, WORD_LEN, 0}, 	// P_B_P4Q0
    {0x3756, 0xB413, WORD_LEN, 0}, 	// P_B_P4Q1
    {0x3758, 0xE395, WORD_LEN, 0}, 	// P_B_P4Q2
    {0x375A, 0x1A37, WORD_LEN, 0}, 	// P_B_P4Q3
    {0x375C, 0x8878, WORD_LEN, 0}, 	// P_B_P4Q4
    {0x365E, 0x0590, WORD_LEN, 0}, 	// P_G2_P0Q0
    {0x3660, 0x8D8F, WORD_LEN, 0}, 	// P_G2_P0Q1
    {0x3662, 0x3CD1, WORD_LEN, 0}, 	// P_G2_P0Q2
    {0x3664, 0x37F1, WORD_LEN, 0}, 	// P_G2_P0Q3
    {0x3666, 0xD012, WORD_LEN, 0}, 	// P_G2_P0Q4
    {0x369E, 0x8BEE, WORD_LEN, 0}, 	// P_G2_P1Q0
    {0x36A0, 0x0030, WORD_LEN, 0}, 	// P_G2_P1Q1
    {0x36A2, 0x4732, WORD_LEN, 0}, 	// P_G2_P1Q2
    {0x36A4, 0xFB4F, WORD_LEN, 0}, 	// P_G2_P1Q3
    {0x36A6, 0xCB93, WORD_LEN, 0}, 	// P_G2_P1Q4
    {0x36DE, 0x1B12, WORD_LEN, 0}, 	// P_G2_P2Q0
    {0x36E0, 0xC24F, WORD_LEN, 0}, 	// P_G2_P2Q1
    {0x36E2, 0xCF33, WORD_LEN, 0}, 	// P_G2_P2Q2
    {0x36E4, 0xAC74, WORD_LEN, 0}, 	// P_G2_P2Q3
    {0x36E6, 0x5B36, WORD_LEN, 0}, 	// P_G2_P2Q4
    {0x371E, 0x04D1, WORD_LEN, 0}, 	// P_G2_P3Q0
    {0x3720, 0xFE33, WORD_LEN, 0}, 	// P_G2_P3Q1
    {0x3722, 0x8F56, WORD_LEN, 0}, 	// P_G2_P3Q2
    {0x3724, 0x2D95, WORD_LEN, 0}, 	// P_G2_P3Q3
    {0x3726, 0x0E57, WORD_LEN, 0}, 	// P_G2_P3Q4
    {0x375E, 0xA2D3, WORD_LEN, 0}, 	// P_G2_P4Q0
    {0x3760, 0x9DF3, WORD_LEN, 0}, 	// P_G2_P4Q1
    {0x3762, 0x9B76, WORD_LEN, 0}, 	// P_G2_P4Q2
    {0x3764, 0x0C77, WORD_LEN, 0}, 	// P_G2_P4Q3
    {0x3766, 0xC717, WORD_LEN, 0}, 	// P_G2_P4Q4
    {0x3784, 0x0340, WORD_LEN, 0}, 	// CENTER_COLUMN
    {0x3782, 0x0360, WORD_LEN, 0}, 	// CENTER_ROW
    {0x3210, 0x01B8, WORD_LEN, 0}, 	// COLOR_PIPELINE_CONTROL
};

struct mt9t11x_reg_t mt9t111_regs = {
    .pll_tbl                = pll_tbl,
    .pll_tbl_sz             = ARRAY_SIZE(pll_tbl),

    .clk_tbl                = clk_tbl,
    .clk_tbl_sz             = ARRAY_SIZE(clk_tbl),

    .prevsnap_tbl           = prev_snap_tbl,
    .prevsnap_tbl_sz        = ARRAY_SIZE(prev_snap_tbl),

    .wb_cloudy_tbl          = wb_cloudy_tbl,
    .wb_cloudy_tbl_sz       = ARRAY_SIZE(wb_cloudy_tbl),

    .wb_daylight_tbl        = wb_daylight_tbl,
    .wb_daylight_tbl_sz     = ARRAY_SIZE(wb_daylight_tbl),

    .wb_flourescant_tbl     = wb_flourescant_tbl,
    .wb_flourescant_tbl_sz  = ARRAY_SIZE(wb_flourescant_tbl),

    .wb_incandescent_tbl    = wb_incandescent_tbl,
    .wb_incandescent_tbl_sz = ARRAY_SIZE(wb_incandescent_tbl),

    .wb_auto_tbl            = wb_auto_tbl,
    .wb_auto_tbl_sz         = ARRAY_SIZE(wb_auto_tbl),

    .af_tbl                 = af_tbl,
    .af_tbl_sz              = ARRAY_SIZE(af_tbl),

    .contrast_tbl           = contrast_tbl,
    .contrast_tbl_sz        = contrast_tbl_sz,

    .brightness_tbl         = brightness_tbl,
    .brightness_tbl_sz      = brightness_tbl_sz,

    .saturation_tbl         = saturation_tbl,
    .saturation_tbl_sz      = saturation_tbl_sz,

    .sharpness_tbl          = sharpness_tbl,
    .sharpness_tbl_sz       = sharpness_tbl_sz,

    .exposure_tbl           = exposure_tbl,
    .exposure_tbl_sz        = exposure_tbl_sz,

    .lens_for_outdoor_tbl   = lens_for_outdoor_tbl,
    .lens_for_outdoor_tbl_sz= ARRAY_SIZE(lens_for_outdoor_tbl),

    .lens_for_indoor_tbl    = lens_for_indoor_tbl,
    .lens_for_indoor_tbl_sz = ARRAY_SIZE(lens_for_indoor_tbl),
};

