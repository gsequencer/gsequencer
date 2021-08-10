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

#ifndef __AGS_VST_SPEAKER_H__
#define __AGS_VST_SPEAKER_H__

#include <glib.h>

#include <ags/vst3-capi/pluginterfaces/vst/ags_vst_types.h>

#ifdef __cplusplus
extern "C" {
#endif

  const AgsVstSpeaker ags_vst_speaker_kspeaker_l    = 1;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_r    = 1 << 1;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_c    = 1 << 2;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_lfe  = 1 << 3;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_ls   = 1 << 4;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_rs   = 1 << 5;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_lc   = 1 << 6;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_rc   = 1 << 7;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_s    = 1 << 8;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_cs   = ags_vst_speaker_kspeaker_s;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_sl   = 1 << 9;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_sr   = 1 << 10;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_tc   = 1 << 11;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_tfl  = 1 << 12;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_tfc  = 1 << 13;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_tfr  = 1 << 14;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_trl  = 1 << 15;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_trc  = 1 << 16;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_trr  = 1 << 17;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_lfe2 = 1 << 18;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_m    = 1 << 19;

  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn0  = (AgsVstSpeaker)1 << 20;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn1  = (AgsVstSpeaker)1 << 21;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn2  = (AgsVstSpeaker)1 << 22;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn3  = (AgsVstSpeaker)1 << 23;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn4  = (AgsVstSpeaker)1 << 38;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn5  = (AgsVstSpeaker)1 << 39;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn6  = (AgsVstSpeaker)1 << 40;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn7  = (AgsVstSpeaker)1 << 41;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn8  = (AgsVstSpeaker)1 << 42;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn9  = (AgsVstSpeaker)1 << 43;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn10 = (AgsVstSpeaker)1 << 44;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn11 = (AgsVstSpeaker)1 << 45;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn12 = (AgsVstSpeaker)1 << 46;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn13 = (AgsVstSpeaker)1 << 47;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn14 = (AgsVstSpeaker)1 << 48;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_acn15 = (AgsVstSpeaker)1 << 49;
  
  const AgsVstSpeaker ags_vst_speaker_kspeaker_tsl = (AgsVstSpeaker)1 << 24;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_tsr = (AgsVstSpeaker)1 << 25;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_lcs = (AgsVstSpeaker)1 << 26;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_rcs = (AgsVstSpeaker)1 << 27;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_bfl = (AgsVstSpeaker)1 << 28;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_bfc = (AgsVstSpeaker)1 << 29;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_bfr = (AgsVstSpeaker)1 << 30;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_pl  = (AgsVstSpeaker)1 << 31;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_pr  = (AgsVstSpeaker)1 << 32;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_bsl = (AgsVstSpeaker)1 << 33;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_bsr = (AgsVstSpeaker)1 << 34;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_brl = (AgsVstSpeaker)1 << 35;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_brc = (AgsVstSpeaker)1 << 36;
  const AgsVstSpeaker ags_vst_speaker_kspeaker_brr = (AgsVstSpeaker)1 << 37;

  const AgsVstSpeakerArrangement ags_vst_speaker_kempty                   = 0;
  const AgsVstSpeakerArrangement ags_vst_speaker_kmono                    = ags_vst_speaker_kspeaker_m;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo                  = ags_vst_speaker_kspeaker_l   | ags_vst_speaker_kspeaker_r;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_surround         = ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_center           = ags_vst_speaker_kspeaker_lc  | ags_vst_speaker_kspeaker_rc;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_side             = ags_vst_speaker_kspeaker_sl  | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_cLfe             = ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_tf               = ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_ts               = ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_tr               = ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_bf               = ags_vst_speaker_kspeaker_bfl | ags_vst_speaker_kspeaker_bfr;
  const AgsVstSpeakerArrangement ags_vst_speaker_kcine_front              = ags_vst_speaker_kspeaker_l   | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc;

  const AgsVstSpeakerArrangement ags_vst_speaker_k30_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c;
  const AgsVstSpeakerArrangement ags_vst_speaker_k31_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe;
  const AgsVstSpeakerArrangement ags_vst_speaker_k30_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k31_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k40_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k41_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k40_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k41_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k50                      = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k51                      = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k60_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k61_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k60_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs  | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k61_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k70_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_full_front      = ags_vst_speaker_k71_cine;
  const AgsVstSpeakerArrangement ags_vst_speaker_k70_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;

  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_full_rear       = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lcs | ags_vst_speaker_kspeaker_rcs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_side_fill       = ags_vst_speaker_k71_music;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_proximity            = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_pl | ags_vst_speaker_kspeaker_pr;

  const AgsVstSpeakerArrangement ags_vst_speaker_k80_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k81_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_cs;
  const AgsVstSpeakerArrangement ags_vst_speaker_k80_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_cs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k81_music                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_cs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k90_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;

  const AgsVstSpeakerArrangement ags_vst_speaker_k91_cine                 = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k100_cine                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_cs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k101_cine                = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c   | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_cs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr;

  const AgsVstSpeakerArrangement ags_vst_speaker_kambi_1st_order_acn      = ags_vst_speaker_kspeaker_acn0 | ags_vst_speaker_kspeaker_acn1 | ags_vst_speaker_kspeaker_acn2 | ags_vst_speaker_kspeaker_acn3;
  const AgsVstSpeakerArrangement ags_vst_speaker_kambi_2cd_order_acn      = ags_vst_speaker_kambi_1st_order_acn | ags_vst_speaker_kspeaker_acn4 | ags_vst_speaker_kspeaker_acn5 | ags_vst_speaker_kspeaker_acn6 | ags_vst_speaker_kspeaker_acn7 | ags_vst_speaker_kspeaker_acn8;
  const AgsVstSpeakerArrangement ags_vst_speaker_kambi_3rd_order_acn      = ags_vst_speaker_kambi_2cd_order_acn | ags_vst_speaker_kspeaker_acn9 | ags_vst_speaker_kspeaker_acn10 | ags_vst_speaker_kspeaker_acn11 | ags_vst_speaker_kspeaker_acn12 | ags_vst_speaker_kspeaker_acn13 | ags_vst_speaker_kspeaker_acn14 | ags_vst_speaker_kspeaker_acn15;

  const AgsVstSpeakerArrangement ags_vst_speaker_k80_cube                 = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs  | ags_vst_speaker_kspeaker_tfl| ags_vst_speaker_kspeaker_tfr| ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_top_center      = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_cs  | ags_vst_speaker_kspeaker_tc;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_center_high     = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_cs  | ags_vst_speaker_kspeaker_tfc;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_front_high      = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_mpeg3d               = ags_vst_speaker_k71_cine_front_high;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_side_high       = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k81_mpeg3d               = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_bfc;
  const AgsVstSpeakerArrangement ags_vst_speaker_k90                      = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tfl| ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k50_4                    = ags_vst_speaker_k90;
  const AgsVstSpeakerArrangement ags_vst_speaker_k91                      = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs  | ags_vst_speaker_kspeaker_tfl| ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k51_4                    = ags_vst_speaker_k91;
  const AgsVstSpeakerArrangement ags_vst_speaker_k70_2                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_2                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k91_atmos                = ags_vst_speaker_k71_2;
  const AgsVstSpeakerArrangement ags_vst_speaker_k70_4                    = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;

  const AgsVstSpeakerArrangement ags_vst_speaker_k71_4                    = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k111_mpeg3d              = ags_vst_speaker_k71_4;

  const AgsVstSpeakerArrangement ags_vst_speaker_k70_6                   = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  
  const AgsVstSpeakerArrangement ags_vst_speaker_k71_6                   = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k90_4                   = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k91_4                   = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k90_6                   = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k91_6                   = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k100                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k101                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs  | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k101_mpeg3d             = ags_vst_speaker_k101;                                                                                                                                                                                       
  const AgsVstSpeakerArrangement ags_vst_speaker_k102                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs  | ags_vst_speaker_kspeaker_tfl| ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_lfe2;
  const AgsVstSpeakerArrangement ags_vst_speaker_k110                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k111                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k122                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs  | ags_vst_speaker_kspeaker_lc  | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_tfl| ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_lfe2;
  const AgsVstSpeakerArrangement ags_vst_speaker_k130                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_ls  | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;      
  const AgsVstSpeakerArrangement ags_vst_speaker_k131                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k140                    = ags_vst_speaker_kspeaker_l | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_bfl | ags_vst_speaker_kspeaker_bfr | ags_vst_speaker_kspeaker_brl | ags_vst_speaker_kspeaker_brr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k220                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_cs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trc | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr | ags_vst_speaker_kspeaker_bfl| ags_vst_speaker_kspeaker_bfc | ags_vst_speaker_kspeaker_bfr;
  const AgsVstSpeakerArrangement ags_vst_speaker_k222                    = ags_vst_speaker_kspeaker_l  | ags_vst_speaker_kspeaker_r | ags_vst_speaker_kspeaker_c  | ags_vst_speaker_kspeaker_lfe | ags_vst_speaker_kspeaker_ls | ags_vst_speaker_kspeaker_rs | ags_vst_speaker_kspeaker_lc | ags_vst_speaker_kspeaker_rc | ags_vst_speaker_kspeaker_cs | ags_vst_speaker_kspeaker_sl | ags_vst_speaker_kspeaker_sr | ags_vst_speaker_kspeaker_tc | ags_vst_speaker_kspeaker_tfl | ags_vst_speaker_kspeaker_tfc | ags_vst_speaker_kspeaker_tfr | ags_vst_speaker_kspeaker_trl | ags_vst_speaker_kspeaker_trc | ags_vst_speaker_kspeaker_trr | ags_vst_speaker_kspeaker_lfe2 | ags_vst_speaker_kspeaker_tsl | ags_vst_speaker_kspeaker_tsr | ags_vst_speaker_kspeaker_bfl| ags_vst_speaker_kspeaker_bfc | ags_vst_speaker_kspeaker_bfr;
  
  const AgsVstCString ags_vst_speaker_kstring_empty              = "";
  const AgsVstCString ags_vst_speaker_kstring_mono               = "Mono";
  const AgsVstCString ags_vst_speaker_kstring_stereo             = "Stereo";
  const AgsVstCString ags_vst_speaker_kstring_stereo_r           = "Stereo (Ls Rs)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_c           = "Stereo (Lc Rc)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_side        = "Stereo (Sl Sr)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_clfe        = "Stereo (C LFE)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_tf          = "Stereo (Tfl Tfr)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_ts          = "Stereo (Tsl Tsr)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_tr          = "Stereo (Trl Trr)";
  const AgsVstCString ags_vst_speaker_kstring_stereo_bf          = "Stereo (Bfl Bfr)";
  const AgsVstCString ags_vst_speaker_kstring_cine_front         = "Cine Front";

  const AgsVstCString ags_vst_speaker_kstring_30_cine            = "LRC";
  const AgsVstCString ags_vst_speaker_kstring_30_music           = "LRS";
  const AgsVstCString ags_vst_speaker_kstring_31_cine            = "LRC+LFE";
  const AgsVstCString ags_vst_speaker_kstring_31_music           = "LRS+LFE";
  const AgsVstCString ags_vst_speaker_kstring_40_cine            = "LRCS";
  const AgsVstCString ags_vst_speaker_kstring_40_music           = "Quadro";
  const AgsVstCString ags_vst_speaker_kstring_41_cine            = "LRCS+LFE";
  const AgsVstCString ags_vst_speaker_kstring_41_music           = "Quadro+LFE";
  const AgsVstCString ags_vst_speaker_kstring_50                 = "5.0";
  const AgsVstCString ags_vst_speaker_kstring_51                 = "5.1";
  const AgsVstCString ags_vst_speaker_kstring_60_cine            = "6.0 Cine";
  const AgsVstCString ags_vst_speaker_kstring_60_usic            = "6.0 Music";
  const AgsVstCString ags_vst_speaker_kstring_61_cine            = "6.1 Cine";
  const AgsVstCString ags_vst_speaker_kstring_61_music           = "6.1 Music";
  const AgsVstCString ags_vst_speaker_kstring_70_cine            = "7.0 SDDS";
  const AgsVstCString ags_vst_speaker_kstring_70_cine_old        = "7.0 Cine (SDDS)";
  const AgsVstCString ags_vst_speaker_kstring_70_music           = "7.0";
  const AgsVstCString ags_vst_speaker_kstring_70_music_old       = "7.0 Music (Dolby)";
  const AgsVstCString ags_vst_speaker_kstring_71_cine            = "7.1 SDDS";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_old        = "7.1 Cine (SDDS)";
  const AgsVstCString ags_vst_speaker_kstring_71_music           = "7.1";
  const AgsVstCString ags_vst_speaker_kstring_71_music_old       = "7.1 Music (Dolby)";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_top_center    = "7.1 Cine Top Center";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_center_high   = "7.1 Cine Center High";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_front_high    = "7.1 Cine Front High";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_side_high     = "7.1 Cine Side High";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_full_rear     = "7.1 Cine Full Rear";
  const AgsVstCString ags_vst_speaker_kstring_71_proximity          = "7.1 Proximity";
  const AgsVstCString ags_vst_speaker_kstring_80_cine           = "8.0 Cine";
  const AgsVstCString ags_vst_speaker_kstring_80_music          = "8.0 Music";
  const AgsVstCString ags_vst_speaker_kstring_80_cube           = "8.0 Cube";
  const AgsVstCString ags_vst_speaker_kstring_81_cine           = "8.1 Cine";
  const AgsVstCString ags_vst_speaker_kstring_81_music          = "8.1 Music";
  const AgsVstCString ags_vst_speaker_kstring_90_cine           = "9.0 Cine";
  const AgsVstCString ags_vst_speaker_kstring_91_cine           = "9.1 Cine";
  const AgsVstCString ags_vst_speaker_kstring_100_cine          = "10.0 Cine";
  const AgsVstCString ags_vst_speaker_kstring_101_cine          = "10.1 Cine";
  const AgsVstCString ags_vst_speaker_kstring_102               = "10.2 Experimental";
  const AgsVstCString ags_vst_speaker_kstring_122               = "12.2";
  const AgsVstCString ags_vst_speaker_kstring_50_4              = "5.0.4";
  const AgsVstCString ags_vst_speaker_kstring_51_4              = "5.1.4";
  const AgsVstCString ags_vst_speaker_kstring_70_2              = "7.0.2";
  const AgsVstCString ags_vst_speaker_kstring_71_2              = "7.1.2";
  const AgsVstCString ags_vst_speaker_kstring_70_4              = "7.0.4";
  const AgsVstCString ags_vst_speaker_kstring_71_4              = "7.1.4";
  const AgsVstCString ags_vst_speaker_kstring_70_6              = "7.0.6";
  const AgsVstCString ags_vst_speaker_kstring_71_6              = "7.1.6";
  const AgsVstCString ags_vst_speaker_kstring_90_4              = "9.0.4";
  const AgsVstCString ags_vst_speaker_kstring_91_4              = "9.1.4";
  const AgsVstCString ags_vst_speaker_kstring_90_6              = "9.0.6";
  const AgsVstCString ags_vst_speaker_kstring_91_6              = "9.1.6";
  const AgsVstCString ags_vst_speaker_kstring_100               = "10.0 Auro-3D";
  const AgsVstCString ags_vst_speaker_kstring_101               = "10.1 Auro-3D";
  const AgsVstCString ags_vst_speaker_kstring_110               = "11.0 Auro-3D";
  const AgsVstCString ags_vst_speaker_kstring_111               = "11.1 Auro-3D";
  const AgsVstCString ags_vst_speaker_kstring_130               = "13.0 Auro-3D";
  const AgsVstCString ags_vst_speaker_kstring_131               = "13.1 Auro-3D";
  const AgsVstCString ags_vst_speaker_kstring_81_mpeg           = "8.1 MPEG";
  const AgsVstCString ags_vst_speaker_kstring_140               = "14.0";
  const AgsVstCString ags_vst_speaker_kstring_222               = "22.2";
  const AgsVstCString ags_vst_speaker_kstring_220               = "22.0";
  const AgsVstCString ags_vst_speaker_kstring_ambi_1st_order    = "1st Order Ambisonics";
  const AgsVstCString ags_vst_speaker_kstring_ambi_2cd_order    = "2nd Order Ambisonics";
  const AgsVstCString ags_vst_speaker_kstring_ambi_3rd_order    = "3rd Order Ambisonics";

  const AgsVstCString ags_vst_speaker_kstring_mono_s              = "M";
  const AgsVstCString ags_vst_speaker_kstring_stereo_s            = "L R";
  const AgsVstCString ags_vst_speaker_kstring_stereo_rw           = "Ls Rs";
  const AgsVstCString ags_vst_speaker_kstring_stereo_cs           = "Lc Rc";
  const AgsVstCString ags_vst_speaker_kstring_stereo_ss           = "Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_stereo_clfes        = "C LFE";
  const AgsVstCString ags_vst_speaker_kstring_stereo_tfs          = "Tfl Tfr";
  const AgsVstCString ags_vst_speaker_kstring_stereo_tss          = "Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_stereo_trs          = "Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_stereo_bfs          = "Bfl Bfr";
  const AgsVstCString ags_vst_speaker_kstring_cine_front_s        = "L R C Lc Rc";
  const AgsVstCString ags_vst_speaker_kstring_30_cine_s           = "L R C";
  const AgsVstCString ags_vst_speaker_kstring_30_music_s          = "L R S";
  const AgsVstCString ags_vst_speaker_kstring_31_cine_s           = "L R C LFE";
  const AgsVstCString ags_vst_speaker_kstring_31_music_s          = "L R LFE S";
  const AgsVstCString ags_vst_speaker_kstring_40_cine_s           = "L R C S";
  const AgsVstCString ags_vst_speaker_kstring_40_music_s          = "L R Ls Rs";
  const AgsVstCString ags_vst_speaker_kstring_41_cine_s           = "L R C LFE S";
  const AgsVstCString ags_vst_speaker_kstring_41_music_s          = "L R LFE Ls Rs";
  const AgsVstCString ags_vst_speaker_kstring_50_s                = "L R C Ls Rs";
  const AgsVstCString ags_vst_speaker_kstring_51_s                = "L R C LFE Ls Rs";
  const AgsVstCString ags_vst_speaker_kstring_60_cine_s           = "L R C Ls Rs Cs";
  const AgsVstCString ags_vst_speaker_kstring_60_music_s          = "L R Ls Rs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_61_cine_s           = "L R C LFE Ls Rs Cs";
  const AgsVstCString ags_vst_speaker_kstring_61_music_s          = "L R LFE Ls Rs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_70_cine_s           = "L R C Ls Rs Lc Rc";
  const AgsVstCString ags_vst_speaker_kstring_70_music_s          = "L R C Ls Rs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_s           = "L R C LFE Ls Rs Lc Rc";
  const AgsVstCString ags_vst_speaker_kstring_71_music_s          = "L R C LFE Ls Rs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_80_cine_s           = "L R C Ls Rs Lc Rc Cs";
  const AgsVstCString ags_vst_speaker_kstring_80_music_s          = "L R C Ls Rs Cs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_81_cine_s           = "L R C LFE Ls Rs Lc Rc Cs";
  const AgsVstCString ags_vst_speaker_kstring_81_music_s          = "L R C LFE Ls Rs Cs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_80_cube_s           = "L R Ls Rs Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_top_center_s   = "L R C LFE Ls Rs Cs Tc";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_center_high_s  = "L R C LFE Ls Rs Cs Tfc";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_front_high_s  = "L R C LFE Ls Rs Tfl Tfl";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_side_high_s    = "L R C LFE Ls Rs Tsl Tsl";
  const AgsVstCString ags_vst_speaker_kstring_71_cine_full_rear_s    = "L R C LFE Ls Rs Lcs Rcs";
  const AgsVstCString ags_vst_speaker_kstring_71_proximity_s         = "L R C LFE Ls Rs Pl Pr";
  const AgsVstCString ags_vst_speaker_kstring_90_cine_s          = "L R C Ls Rs Lc Rc Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_91_cine_s          = "L R C Lfe Ls Rs Lc Rc Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_100_cine_s         = "L R C Ls Rs Lc Rc Cs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_101_cine_s         = "L R C Lfe Ls Rs Lc Rc Cs Sl Sr";
  const AgsVstCString ags_vst_speaker_kstring_50_4s              = "L R C Ls Rs Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_51_4s              = "L R C LFE Ls Rs Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_70_2s              = "L R C Ls Rs Sl Sr Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_71_2s              = "L R C LFE Ls Rs Sl Sr Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_70_4s              = "L R C Ls Rs Sl Sr Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_71_4s              = "L R C LFE Ls Rs Sl Sr Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_70_6s              = "L R C Ls Rs Sl Sr Tfl Tfr Trl Trr Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_71_6s              = "L R C LFE Ls Rs Sl Sr Tfl Tfr Trl Trr Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_90_4s              = "L R C Ls Rs Lc Rc Sl Sr Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_91_4s              = "L R C LFE Ls Rs Lc Rc Sl Sr Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_90_6s              = "L R C Ls Rs Lc Rc Sl Sr Tfl Tfr Trl Trr Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_91_6s              = "L R C LFE Ls Rs Lc Rc Sl Sr Tfl Tfr Trl Trr Tsl Tsr";
  const AgsVstCString ags_vst_speaker_kstring_100_s              = "L R C Ls Rs Tc Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_101_s              = "L R C LFE Ls Rs Tc Tfl Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_110_s              = "L R C Ls Rs Tc Tfl Tfc Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_111_s              = "L R C LFE Ls Rs Tc Tfl Tfc Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_130_s              = "L R C Ls Rs Sl Sr Tc Tfl Tfc Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_131_s              = "L R C LFE Ls Rs Sl Sr Tc Tfl Tfc Tfr Trl Trr";
  const AgsVstCString ags_vst_speaker_kstring_102_s              = "L R C LFE Ls Rs Tfl Tfc Tfr Trl Trr LFE2";
  const AgsVstCString ags_vst_speaker_kstring_122_s              = "L R C LFE Ls Rs Lc Rc Tfl Tfc Tfr Trl Trr LFE2";
  const AgsVstCString ags_vst_speaker_kstring_81_mpegs           = "L R LFE Ls Rs Tfl Tfc Tfr Bfc";
  const AgsVstCString ags_vst_speaker_kstring_140_s              = "L R Ls Rs Sl Sr Tfl Tfr Trl Trr Bfl Bfr Brl Brr";
  const AgsVstCString ags_vst_speaker_kstring_222_s              = "L R C LFE Ls Rs Lc Rc Cs Sl Sr Tc Tfl Tfc Tfr Trl Trc Trr LFE2 Tsl Tsr Bfl Bfc Bfr";
  const AgsVstCString ags_vst_speaker_kstring_220_s              = "L R C Ls Rs Lc Rc Cs Sl Sr Tc Tfl Tfc Tfr Trl Trc Trr Tsl Tsr Bfl Bfc Bfr";

  const AgsVstCString ags_vst_speaker_kstring_ambi_1st_order_s   = "0 1 2 3";
  const AgsVstCString ags_vst_speaker_kstring_ambi_2cd_order_s   = "0 1 2 3 4 5 6 7 8";
  const AgsVstCString ags_vst_speaker_kstring_ambi_3rd_order_s   = "0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15";

  gint32 ags_vst_speaker_get_channel_count(AgsVstSpeakerArrangement arr);

  gint32 ags_vst_speaker_get_speaker_index(AgsVstSpeaker speaker, AgsVstSpeakerArrangement arr);
  
  AgsVstSpeaker ags_vst_speaker_get_speaker(AgsVstSpeakerArrangement *arr, gint32 index);

  gboolean ags_vst_speaker_is_subset_of(AgsVstSpeakerArrangement *arr_sub_set, AgsVstSpeakerArrangement *arr);

  gboolean ags_vst_speaker_is_auro(AgsVstSpeakerArrangement *arr);
  
  gboolean ags_vst_speaker_has_top_speakers(AgsVstSpeakerArrangement *arr);

  gboolean ags_vst_speaker_has_bottom_speakers(AgsVstSpeakerArrangement *arr);

  gboolean ags_vst_speaker_has_middle_speakers(AgsVstSpeakerArrangement *arr);

  gboolean ags_vst_speaker_has_lfe(AgsVstSpeakerArrangement *arr);

  gboolean ags_vst_speaker_is_3d(AgsVstSpeakerArrangement *arr);

  gboolean ags_vst_speaker_is_ambisonics(AgsVstSpeakerArrangement *arr);

  AgsVstSpeakerArrangement ags_vst_speaker_get_speaker_arrangement_from_string(AgsVstCString arr_str);

  AgsVstCString ags_vst_speaker_get_speaker_arrangement_string(AgsVstSpeakerArrangement arr,
							       gboolean with_speakers_name);

  AgsVstCString ags_vst_speaker_get_speaker_short_name(AgsVstSpeakerArrangement *arr,
						       gint32 index);  
  
#ifdef __cplusplus
}
#endif

#endif /*__AGS_VST_SPEAKER_H__*/
