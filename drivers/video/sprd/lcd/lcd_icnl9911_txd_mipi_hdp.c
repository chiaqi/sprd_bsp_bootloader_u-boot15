/*
 * Copyright (C) 2018 Spreadtrum Communications Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "sprd_panel.h"
#include "sprd_dsi.h"
#include "dsi/mipi_dsi_api.h"
#include "sprd_dphy.h"

static uint8_t init_data[] = {

	0x39, 0x00, 0x00, 0x03, 0xF0, 0x5A, 0x5A,
	0x39, 0x00, 0x00, 0x03, 0xF1, 0xA5, 0xA5,
	0x15, 0x00, 0x00, 0x02, 0xC2, 0x00,//TE
	//GSP
	0x39, 0x00, 0x00, 0x11, 0xB0, 0x76, 0x54, 0x11, 0x11, 0x33, 0x33, 0x33, 0x33, 0x00, 0x01, 0x01, 0x76, 0x01, 0x01, 0x00, 0x00,
	//GCK
	0x39, 0x00, 0x00, 0x0E, 0xB1, 0x53, 0xD4, 0x02, 0x85, 0x00, 0x01, 0x01, 0x76, 0x01, 0x01, 0x53, 0x5F, 0x5F,
	0x39, 0x00, 0x00, 0x11, 0xB2, 0x37, 0x20, 0x05, 0x80, 0x65, 0x02, 0x08, 0x20, 0x30, 0x91, 0x22, 0x33, 0x44, 0x00, 0x18, 0x01,
	0x39, 0x00, 0x00, 0x11, 0xB3, 0x0F, 0x00, 0x87, 0x10, 0x80, 0x26, 0x26, 0xC0, 0x3F, 0xAA, 0x33, 0xC3, 0xAA, 0x30, 0xC3, 0xAA,
	//GOUTL (BOE: nc(1)  GCL VSD STV3 STV1 VGL CLK1 CLK3 CLK5 CLK7 GCH VDS)
	0x39, 0x00, 0x00, 0x0D, 0xB4, 0x03, 0x23, 0x14, 0x05, 0x07, 0x1B, 0x0F, 0x0D, 0x13, 0x11, 0x24, 0x15,
	//GOUTR (BOE: nc(1)  GCL VSD STV4 STV2 VGL CLK2 CLK4 CLK6 CLK8 GCH VDS)
	0x39, 0x00, 0x00, 0x0D, 0xB6, 0x03, 0x23, 0x14, 0x04, 0x06, 0x1B, 0x0E, 0x0C, 0x12, 0x10, 0x24, 0x15,
	//	0x39, 0x00, 0x00, 0x03, 0xB9, 0x5a, 0x5a,  //VCOM
	0x39, 0x00, 0x00, 0x11, 0xBB, 0x00, 0x00, 0x00, 0x00, 0x02, 0xFF, 0xFD, 0x0B, 0x33, 0x01, 0x73, 0x33, 0x33, 0x00, 0x00, 0x00,
	0x39, 0x00, 0x00, 0x0B, 0xBC, 0x61, 0x03, 0xFF, 0xDE, 0x72, 0xE0, 0x2E, 0x04, 0x88, 0x3E,		//enable GIP VGH in APO-0914
	0x39, 0x00, 0x00, 0x11, 0xBD, 0x8E, 0x0E, 0x78, 0x78, 0x15, 0x15, 0x46, 0x5A, 0x14, 0x66, 0x23, 0x06, 0x00, 0x00, 0x00, 0x00,			//VGMP 5.0V-0911 VGH=14V VGL=-14V
	0x39, 0x00, 0x00, 0x06, 0xBE, 0x60, 0x60, 0x50, 0x60, 0x77,
	0x39, 0x00, 0x00, 0x0F, 0xC1, 0x70, 0x7C, 0x3A, 0x78, 0x04, 0x0C, 0x10, 0x04, 0x2A, 0x71, 0x00, 0x07, 0x10, 0x10,
	0x39, 0x00, 0x00, 0x0A, 0xC3, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0x0D, 0x1F,
	0x39, 0x00, 0x00, 0x09, 0xC4, 0xB4, 0xA3, 0xEE, 0x41, 0x04, 0x2F, 0x00, 0x00,
	0x39, 0x00, 0x00, 0x11, 0xC5, 0x07, 0x1F, 0x42, 0x26, 0x51, 0x44, 0x14, 0x1A, 0x04, 0x00, 0x0A, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x39, 0x00, 0x00, 0x08, 0xC6, 0x85, 0x01, 0x67, 0x01, 0x33, 0x00, 0x03,
	//gamma
	0x39, 0x00, 0x00, 0x11, 0xC7, 0x7C, 0x72, 0x69, 0x62, 0x71, 0x61, 0x55, 0x5C, 0x46, 0x5D, 0x59, 0x40, 0x54, 0x4D, 0x5B, 0x4B,
	0x39, 0x00, 0x00, 0x06, 0xC8, 0x35, 0x60, 0x37, 0x25, 0x0C,
	0x39, 0x00, 0x00, 0x11, 0xC9, 0x7C, 0x72, 0x69, 0x62, 0x71, 0x61, 0x55, 0x5C, 0x46, 0x5D, 0x59, 0x40, 0x55, 0x4D, 0x5B, 0x4B,
	0x39, 0x00, 0x00, 0x06, 0xCA, 0x35, 0x60, 0x37, 0x25, 0x0C,
	0x39, 0x00, 0x00, 0x0C, 0xCB, 0x00, 0x00, 0x00, 0x01, 0x6C, 0x00, 0x33, 0x00, 0x17, 0xFF, 0xEF,
	0x39, 0x00, 0x00, 0x03, 0xF0, 0xB4, 0x4B,
	0x39, 0x00, 0x00, 0x09, 0xD0, 0x80, 0x0D, 0xFF, 0x0F, 0x63, 0x2B, 0x08, 0x88,
	0x39, 0x00, 0x00, 0x0B, 0xD2, 0x43, 0x0C, 0x00, 0x01, 0x80, 0x26, 0x04, 0x00, 0x16, 0x01,
	0x15, 0x00, 0x00, 0x02, 0x35, 0x00,
	0x39, 0x00, 0x00, 0x03, 0xF0, 0xA5, 0xA5,
	0x39, 0x00, 0x00, 0x03, 0xF1, 0x5A, 0x5A,

	0x05, 0x78, 0x00, 0x01, 0x11,
	0x05, 0x0A, 0x00, 0x01, 0x29,
	0x15, 0x00, 0x00, 0x02, 0x26, 0x01,

	CMD_END
};

static int mipi_dsi_send_cmds(struct sprd_dsi *dsi, void *data)
{
	uint16_t len;
	struct dsi_cmd_desc *cmds = data;

	if ((cmds == NULL) || (dsi == NULL))
		return -1;

	for (; cmds->data_type != CMD_END;) {
		len = (cmds->wc_h << 8) | cmds->wc_l;
		mipi_dsi_dcs_write(dsi, cmds->payload, len);
		if (cmds->wait)
			msleep(cmds->wait);
		cmds = (struct dsi_cmd_desc *)(cmds->payload + len);
	}
	return 0;
}

static int icnl9911_init(void)
{
	struct sprd_dsi *dsi = &dsi_device;
	struct sprd_dphy *dphy = &dphy_device;

	mipi_dsi_lp_cmd_enable(dsi, true);
	mipi_dsi_send_cmds(dsi, init_data);
	mipi_dsi_set_work_mode(dsi, SPRD_MIPI_MODE_VIDEO);
	mipi_dsi_state_reset(dsi);
	mipi_dphy_hs_clk_en(dphy, true);

	return 0;
}

static int icnl9911_readid(void)
{
	struct sprd_dsi *dsi = &dsi_device;
	uint8_t read_buf[1] = {0};

	mipi_dsi_lp_cmd_enable(dsi, true);
	mipi_dsi_set_max_return_size(dsi, 1);
	mipi_dsi_dcs_read(dsi, 0x04, &read_buf, 1);
	if(0x01 == read_buf[0]) {
		pr_info("icnl9911 read id success!\n");
		return 0;
        }

	pr_err("icnl9911 read id failed!, 0x%x\n",read_buf[0]);
	return 0;
}

static int icnl9911_power(int on)
{
	if (on) {
		sprd_gpio_request(NULL, CONFIG_LCM_GPIO_RSTN);
		sprd_gpio_direction_output(NULL, CONFIG_LCM_GPIO_RSTN, 1);
		mdelay(5);
		sprd_gpio_direction_output(NULL, CONFIG_LCM_GPIO_RSTN, 0);
		mdelay(5);
		sprd_gpio_direction_output(NULL, CONFIG_LCM_GPIO_RSTN, 1);
		mdelay(20);
	} else {
		sprd_gpio_direction_output(NULL, CONFIG_LCM_GPIO_RSTN, 0);
		mdelay(5);
	}

	return 0;
}

static struct panel_ops icnl9911_ops = {
	.init = icnl9911_init,
	.read_id = icnl9911_readid,
	.power = icnl9911_power,
};

static struct panel_info icnl9911_info = {
	/* common parameters */
	.lcd_name = "lcd_icnl9911_txd_mipi_hdp",
	.type = SPRD_PANEL_TYPE_MIPI,
	.bpp = 24,
//	.fps = 60,
	.width = 720,
	.height = 1520,

	/* DPI specific parameters */
	.pixel_clk = 76800000, /*Hz*/
	.rgb_timing = {
		.hfp = 16,
		.hbp = 12,
		.hsync = 4,
		.vfp = 120,
		.vbp = 58,
		.vsync = 4,
	},

	/* MIPI DSI specific parameters */
	.phy_freq = 600000,
	.lane_num = 4,
	.work_mode = SPRD_MIPI_MODE_VIDEO,
	.burst_mode = PANEL_VIDEO_BURST_MODE,
	.nc_clk_en = false,
};

struct panel_driver icnl9911_txd_driver = {
	.info = &icnl9911_info,
	.ops = &icnl9911_ops,
};
