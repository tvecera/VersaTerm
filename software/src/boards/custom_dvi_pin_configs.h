#ifndef _CUSTOM_DVI_PIN_CONFIGS_H
#define _CUSTOM_DVI_PIN_CONFIGS_H

// ----------------------------------------------------------------------------
// PicoDVI boards

// PicoPad external display module
static const struct dvi_serialiser_cfg picodvi_picopad_extdisp_cfg = {
	.pio = DVI_DEFAULT_PIO_INST,
	.sm_tmds = {0, 1, 2},
	.pins_tmds = {2, 4, 6},
	.pins_clk = 0,
	.invert_diffpairs = true
};

#endif
