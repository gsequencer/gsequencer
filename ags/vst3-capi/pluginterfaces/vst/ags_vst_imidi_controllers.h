/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_VST_IMIDI_CONTROLLERS_H__
#define __AGS_VST_IMIDI_CONTROLLERS_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/base/ags_vst_funknown.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  enum ControllerNumbers
  {
    AGS_VST_KCTRL_BANK_SELECT_MSB       =   0,
    AGS_VST_KCTRL_MOD_WHEEL             =   1,
    AGS_VST_KCTRL_BREATH                =   2,
    AGS_VST_KCTRL_FOOT                  =   4,
    AGS_VST_KCTRL_PORTA_TIME            =   5,
    AGS_VST_KCTRL_DATA_ENTRY_MSB        =   6,
    AGS_VST_KCTRL_VOLUME                =   7,
    AGS_VST_KCTRL_BALANCE               =   8,
    AGS_VST_KCTRL_PAN                   =  10,
    AGS_VST_KCTRL_EXPRESSION            =  11,
    AGS_VST_KCTRL_EFFECT1               =  12,
    AGS_VST_KCTRL_EFFECT2               =  13,
    AGS_VST_KCTRL_GPC1                  =  16,
    AGS_VST_KCTRL_GPC2                  =  17,
    AGS_VST_KCTRL_GPC3                  =  18,
    AGS_VST_KCTRL_GPC4                  =  19,
    AGS_VST_KCTRL_BANK_SELECT_LSB       =  32,
    AGS_VST_KCTRL_DATA_ENTRY_LSB        =  38,
    AGS_VST_KCTRL_SUSTAIN_ON_OFF        =  64,
    AGS_VST_KCTRL_PORTA_ON_OFF          =  65,
    AGS_VST_KCTRL_SUSTENUTO_ON_OFF      =  66,
    AGS_VST_KCTRL_SOFT_PEDAL_ON_OFF     =  67,
    AGS_VST_KCTRL_LEGATO_FOOT_SW_ON_OFF =  68,
    AGS_VST_KCTRL_HOLD_2_ON_OFF         =  69,
    AGS_VST_KCTRL_SOUND_VARIATION       =  70,
    AGS_VST_KCTRL_FILTER_CUT_OFF        =  71,
    AGS_VST_KCTRL_RELEASE_TIME          =  72,
    AGS_VST_KCTRL_ATTACK_TIME           =  73,
    AGS_VST_KCTRL_FILTER_RESONANCE      =  74,
    AGS_VST_KCTRL_DECAY_TIME            =  75,
    AGS_VST_KCTRL_VIBRATO_RATE          =  76,
    AGS_VST_KCTRL_VIBRATO_DEPTH         =  77,
    AGS_VST_KCTRL_VIBRAT_DELAY          =  78,
    AGS_VST_KCTRL_SOUND_CTRL_10         =  79,
    AGS_VST_KCTRL_GPC5                  =  80,
    AGS_VST_KCTRL_GPC6                  =  81,
    AGS_VST_KCTRL_GPC7                  =  82,
    AGS_VST_KCTRL_GPC8                  =  83,
    AGS_VST_KCTRL_PORTA_CONTROL         =  84,
    AGS_VST_KCTRL_EFF1_DEPTH            =  91,
    AGS_VST_KCTRL_EFF2_DEPTH            =  92,
    AGS_VST_KCTRL_EFF3_DEPTH            =  93,
    AGS_VST_KCTRL_EFF4_DEPTH            =  94,
    AGS_VST_KCTRL_EFF5_DEPTH            =  95,
    AGS_VST_KCTRL_DATA_INCREMENT        =  96,
    AGS_VST_KCTRL_DATA_DECREMENT        =  97,
    AGS_VST_KCTRL_NRPN_SELECT_LSB       =  98,
    AGS_VST_KCTRL_NRPN_SELECT_MSB       =  99,
    AGS_VST_KCTRL_RPN_SELECT_LSB        = 100,
    AGS_VST_KCTRL_RPN_SELECT_MSB        = 101,
    AGS_VST_KCTRL_ALL_SOUNDS_OFF        = 120,
    AGS_VST_KCTRL_RESET_ALL_CTRLERS     = 121,
    AGS_VST_KCTRL_LOCAL_CTRL_ON_OFF     = 122,
    AGS_VST_KCTRL_ALL_NOTES_OFF         = 123,
    AGS_VST_KCTRL_OMNI_MODE_OFF         = 124,
    AGS_VST_KCTRL_OMNI_MODE_ON          = 125,
    AGS_VST_KCTRL_POLY_MODE_ON_OFF      = 126,
    AGS_VST_KCTRL_POLY_MODE_ON          = 127,
    AGS_VST_KAFTER_TOUCH                = 128,
    AGS_VST_KPITCH_BEND                 = 129,
    AGS_VST_KCOUNT_CTRL_NUMBER,
    AGS_VST_KCTRL_PROGRAM_CHANGE        = 130,
    AGS_VST_KCTRL_POLY_PRESSURE         = 131,
    AGS_VST_KCTRL_QUARTER_FRAME         = 132,
  };
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_IMIDI_CONTROLLERS_H__*/
