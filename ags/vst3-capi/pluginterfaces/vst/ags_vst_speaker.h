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

  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_l;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_r;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_c;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_lfe;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_ls;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_rs;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_lc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_rc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_s;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_cs;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_sl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_sr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_tc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_tfl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_tfc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_tfr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_trl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_trc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_trr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_lfe2;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_m;

  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn0;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn1;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn2;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn3;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn4;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn5;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn6;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn7;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn8;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn9;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn10;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn11;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn12;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn13;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn14;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_acn15;
  
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_tsl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_tsr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_lcs;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_rcs;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_bfl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_bfc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_bfr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_pl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_pr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_bsl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_bsr;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_brl;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_brc;
  extern const AgsVstSpeaker ags_vst_speaker_kspeaker_brr;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_kempty;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kmono;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_surround;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_center;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_side;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_cLfe;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_tf;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_ts;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_tr;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kstereo_bf;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kcine_front;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k30_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k31_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k30_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k31_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k40_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k41_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k40_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k41_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k50;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k51;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k60_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k61_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k60_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k61_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k70_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_full_front;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k70_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_music;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_full_rear;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_side_fill;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_proximity;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k80_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k81_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k80_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k81_music;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k90_cine;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k91_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k100_cine;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k101_cine;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_kambi_1st_order_acn;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kambi_2cd_order_acn;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_kambi_3rd_order_acn;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k80_cube;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_top_center;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_center_high;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_front_high;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_mpeg3d;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_cine_side_high;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k81_mpeg3d;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k90;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k50_4;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k91;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k51_4;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k70_2;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_2;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k91_atmos;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k70_4;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_4;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k111_mpeg3d;

  extern const AgsVstSpeakerArrangement ags_vst_speaker_k70_6;
  
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k71_6;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k90_4;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k91_4;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k90_6;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k91_6;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k100;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k101;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k101_mpeg3d;                                                                                                                                                                                       
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k102;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k110;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k111;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k122;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k130;      
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k131;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k140;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k220;
  extern const AgsVstSpeakerArrangement ags_vst_speaker_k222;
  
  extern const AgsVstCString ags_vst_speaker_kstring_empty;
  extern const AgsVstCString ags_vst_speaker_kstring_mono;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_r;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_c;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_side;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_clfe;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_tf;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_ts;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_tr;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_bf;
  extern const AgsVstCString ags_vst_speaker_kstring_cine_front;

  extern const AgsVstCString ags_vst_speaker_kstring_30_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_30_music;
  extern const AgsVstCString ags_vst_speaker_kstring_31_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_31_music;
  extern const AgsVstCString ags_vst_speaker_kstring_40_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_40_music;
  extern const AgsVstCString ags_vst_speaker_kstring_41_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_41_music;
  extern const AgsVstCString ags_vst_speaker_kstring_50;
  extern const AgsVstCString ags_vst_speaker_kstring_51;
  extern const AgsVstCString ags_vst_speaker_kstring_60_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_60_usic;
  extern const AgsVstCString ags_vst_speaker_kstring_61_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_61_music;
  extern const AgsVstCString ags_vst_speaker_kstring_70_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_70_cine_old;
  extern const AgsVstCString ags_vst_speaker_kstring_70_music;
  extern const AgsVstCString ags_vst_speaker_kstring_70_music_old;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_old;
  extern const AgsVstCString ags_vst_speaker_kstring_71_music;
  extern const AgsVstCString ags_vst_speaker_kstring_71_music_old;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_top_center;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_center_high;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_front_high;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_side_high;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_full_rear;
  extern const AgsVstCString ags_vst_speaker_kstring_71_proximity;
  extern const AgsVstCString ags_vst_speaker_kstring_80_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_80_music;
  extern const AgsVstCString ags_vst_speaker_kstring_80_cube;
  extern const AgsVstCString ags_vst_speaker_kstring_81_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_81_music;
  extern const AgsVstCString ags_vst_speaker_kstring_90_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_91_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_100_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_101_cine;
  extern const AgsVstCString ags_vst_speaker_kstring_102;
  extern const AgsVstCString ags_vst_speaker_kstring_122;
  extern const AgsVstCString ags_vst_speaker_kstring_50_4;
  extern const AgsVstCString ags_vst_speaker_kstring_51_4;
  extern const AgsVstCString ags_vst_speaker_kstring_70_2;
  extern const AgsVstCString ags_vst_speaker_kstring_71_2;
  extern const AgsVstCString ags_vst_speaker_kstring_70_4;
  extern const AgsVstCString ags_vst_speaker_kstring_71_4;
  extern const AgsVstCString ags_vst_speaker_kstring_70_6;
  extern const AgsVstCString ags_vst_speaker_kstring_71_6;
  extern const AgsVstCString ags_vst_speaker_kstring_90_4;
  extern const AgsVstCString ags_vst_speaker_kstring_91_4;
  extern const AgsVstCString ags_vst_speaker_kstring_90_6;
  extern const AgsVstCString ags_vst_speaker_kstring_91_6;
  extern const AgsVstCString ags_vst_speaker_kstring_100;
  extern const AgsVstCString ags_vst_speaker_kstring_101;
  extern const AgsVstCString ags_vst_speaker_kstring_110;
  extern const AgsVstCString ags_vst_speaker_kstring_111;
  extern const AgsVstCString ags_vst_speaker_kstring_130;
  extern const AgsVstCString ags_vst_speaker_kstring_131;
  extern const AgsVstCString ags_vst_speaker_kstring_81_mpeg;
  extern const AgsVstCString ags_vst_speaker_kstring_140;
  extern const AgsVstCString ags_vst_speaker_kstring_222;
  extern const AgsVstCString ags_vst_speaker_kstring_220;
  extern const AgsVstCString ags_vst_speaker_kstring_ambi_1st_order;
  extern const AgsVstCString ags_vst_speaker_kstring_ambi_2cd_order;
  extern const AgsVstCString ags_vst_speaker_kstring_ambi_3rd_order;

  extern const AgsVstCString ags_vst_speaker_kstring_mono_s;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_s;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_rw;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_cs;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_ss;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_clfes;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_tfs;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_tss;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_trs;
  extern const AgsVstCString ags_vst_speaker_kstring_stereo_bfs;
  extern const AgsVstCString ags_vst_speaker_kstring_cine_front_s;
  extern const AgsVstCString ags_vst_speaker_kstring_30_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_30_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_31_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_31_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_40_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_40_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_41_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_41_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_50_s;
  extern const AgsVstCString ags_vst_speaker_kstring_51_s;
  extern const AgsVstCString ags_vst_speaker_kstring_60_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_60_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_61_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_61_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_70_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_70_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_80_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_80_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_81_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_81_music_s;
  extern const AgsVstCString ags_vst_speaker_kstring_80_cube_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_top_center_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_center_high_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_front_high_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_side_high_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_cine_full_rear_s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_proximity_s;
  extern const AgsVstCString ags_vst_speaker_kstring_90_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_91_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_100_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_101_cine_s;
  extern const AgsVstCString ags_vst_speaker_kstring_50_4s;
  extern const AgsVstCString ags_vst_speaker_kstring_51_4s;
  extern const AgsVstCString ags_vst_speaker_kstring_70_2s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_2s;
  extern const AgsVstCString ags_vst_speaker_kstring_70_4s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_4s;
  extern const AgsVstCString ags_vst_speaker_kstring_70_6s;
  extern const AgsVstCString ags_vst_speaker_kstring_71_6s;
  extern const AgsVstCString ags_vst_speaker_kstring_90_4s;
  extern const AgsVstCString ags_vst_speaker_kstring_91_4s;
  extern const AgsVstCString ags_vst_speaker_kstring_90_6s;
  extern const AgsVstCString ags_vst_speaker_kstring_91_6s;
  extern const AgsVstCString ags_vst_speaker_kstring_100_s;
  extern const AgsVstCString ags_vst_speaker_kstring_101_s;
  extern const AgsVstCString ags_vst_speaker_kstring_110_s;
  extern const AgsVstCString ags_vst_speaker_kstring_111_s;
  extern const AgsVstCString ags_vst_speaker_kstring_130_s;
  extern const AgsVstCString ags_vst_speaker_kstring_131_s;
  extern const AgsVstCString ags_vst_speaker_kstring_102_s;
  extern const AgsVstCString ags_vst_speaker_kstring_122_s;
  extern const AgsVstCString ags_vst_speaker_kstring_81_mpegs;
  extern const AgsVstCString ags_vst_speaker_kstring_140_s;
  extern const AgsVstCString ags_vst_speaker_kstring_222_s;
  extern const AgsVstCString ags_vst_speaker_kstring_220_s;

  extern const AgsVstCString ags_vst_speaker_kstring_ambi_1st_order_s;
  extern const AgsVstCString ags_vst_speaker_kstring_ambi_2cd_order_s;
  extern const AgsVstCString ags_vst_speaker_kstring_ambi_3rd_order_s;

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
