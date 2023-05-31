/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/midi/ags_midi_ci_1_1_util.h>

/**
 * SECTION:ags_midi_ci_1_1_util
 * @short_description: MIDI util
 * @title: AgsMidiUtil
 * @section_id:
 * @include: ags/audio/midi/ags_midi_ci_1_1_util.h
 *
 * Utility functions for MIDI CI version 1.1.
 */

GType
ags_midi_ci_1_1_util_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_ci_1_1_util = 0;

    ags_type_midi_ci_1_1_util =
      g_boxed_type_register_static("AgsMidiCI_1_1_Util",
				   (GBoxedCopyFunc) ags_midi_ci_1_1_util_copy,
				   (GBoxedFreeFunc) ags_midi_ci_1_1_util_free);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_ci_1_1_util);
  }

  return g_define_type_id__volatile;
}

AgsMidiCI_1_1_Util*
ags_midi_ci_1_1_util_alloc()
{
  AgsMidiCI_1_1_Util *midi_ci_1_1_util;

  midi_ci_1_1_util = g_new0(AgsMidiCI_1_1_Util,
			    1);

  midi_ci_1_1_util->rand = g_rand_new();
  
  return(midi_ci_1_1_util);
}

void
ags_midi_ci_1_1_util_free(AgsMidiCI_1_1_Util *midi_ci_1_1_util)
{
  g_return_if_fail(midi_ci_1_1_util == NULL);
  
  g_free(ptr);
}

AgsMidiCI_1_1_Util*
ags_midi_ci_1_1_util_copy(AgsMidiCI_1_1_Util *midi_ci_1_1_util)
{
  AgsMidiCI_1_1_Util *retval;

  g_return_val_if_fail(midi_ci_1_1_util == NULL, NULL);

  retval = ags_midi_ci_1_1_util_alloc();
  
  return(retval);
}

AgsMUID
ags_midi_ci_1_1_util_generate_muid(AgsMidiCI_1_1_Util *midi_ci_1_1_util)
{
  AgsMUID muid;

  g_return_val_if_fail(midi_ci_1_1_util == NULL, 0);

  muid = g_rand_int_range(midi_ci_1_1_util->rand,
			  0,
			  exp2(28));

  return(muid);
}

guint
ags_midi_ci_1_1_util_generate_put_discovery(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					    guchar *buffer,
					    guchar version,
					    AgsMUID source,
					    guint32 manufacturer_id,
					    guint32 device_family,
					    guint32 device_family_model_number,
					    guint32 software_revision_level,
					    guchar capability,
					    guint32 max_sysex_message_size)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_1_1_util == NULL, 0);
  g_return_val_if_fail(buffer == NULL, 0);

  buffer[0] = 0xf0;
  buffer[1] = 0x7e;

  buffer[2] = 0x7f;

  buffer[3] = 0x0d;

  buffer[4] = version;

  buffer[5] = (0xff & source);
  buffer[6] = (0xff00 & source) >> 8;
  buffer[7] = (0xff0000 & source) >> 16;
  buffer[8] = (0xff000000 & source) >> 24;

  /* broadcast */
  buffer[8] = 0x0f;
  buffer[9] = 0xff;
  buffer[10] = 0xff;
  buffer[11] = 0xff;

  nth = 0;

  /* manufacturer */
  buffer[12 + nth] = (0xff & manufacturer_id);
  nth++;
  
  buffer[12 + nth] = 0x0;
  nth++;
  
  buffer[12 + nth] = 0x0;
  nth++;

  /* device family */
  buffer[12 + nth] = (0xff & device_family);
  nth++;
  
  buffer[12 + nth] = (0xff00 & device_family) >> 8;
  nth++;

  /* device family model number */
  buffer[12 + nth] = (0xff & device_family_model_number);
  nth++;
  
  buffer[12 + nth] = (0xff00 & device_family_number) >> 8;
  nth++;

  /* software revision level */
  buffer[12 + nth] = (0xff & software_revision_level);
  nth++;
  
  buffer[12 + nth] = (0xff00 & device_family_number) >> 8;
  nth++;
  
  buffer[12 + nth] = (0xff00 & device_family_number) >> 16;
  nth++;
  
  buffer[12 + nth] = (0xff00 & device_family_number) >> 24;
  nth++;

  /* capability */
  buffer[12 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  buffer[12 + nth] = (0xff & max_sysex_message_size);
  nth++;
  
  buffer[12 + nth] = (0xff & max_sysex_message_size) >> 8;
  nth++;

  buffer[12 + nth] = (0xff & max_sysex_message_size) >> 16;
  nth++;

  buffer[12 + nth] = (0xff & max_sysex_message_size) >> 24;
  nth++;
  
  buffer[12 + nth] = 0xf7;
  nth++;

  return(12 + nth);
}

guint
ags_midi_ci_1_1_util_generate_get_discovery(AgsMidiCI_1_1_Util *midi_ci_1_1_util,
					    guchar *buffer,
					    guchar *version,
					    AgsMUID *source,
					    guint32 *manufacturer_id,
					    guint32 *device_family,
					    guint32 *device_family_model_number,
					    guint32 *software_revision_level,
					    guchar *capability,
					    guint32 *max_sysex_message_size)
{
  guint nth;
  
  g_return_val_if_fail(midi_ci_1_1_util == NULL);
  g_return_val_if_fail(buffer[0] != 0xf0);
  g_return_val_if_fail(buffer[1] != 0x7e);
  g_return_val_if_fail(buffer[2] != 0x7f);
  g_return_val_if_fail(buffer[3] != 0x0d);
  g_return_val_if_fail(buffer[8] != 0x0f || buffer[9] != 0xff || buffer[10] != 0xff || buffer[11] != 0xff);

  /* version */
  if(version != NULL){
    version[0] = buffer[4];
  }

  /* source */
  if(source != NULL){
    source[0] = ((buffer[5]) | (buffer[6] << 8) | (buffer[7] << 16) | (buffer[8] << 24));
  }

  /* destination - broadcast */
  nth = 0;

  /* manufacturer */
  if(manufacturer_id != NULL){
    manufacturer_id[0] = buffer[12 + nth];
  }

  nth++;

  /* device family */
  if(device_family != NULL){
    device_family[0] = ((buffer[12 + nth]) | (buffer[12 + nth + 1] >> 8));
  }
  
  nth += 2;

  /* device family model number */
  if(device_family != NULL){
    device_family_number[0] = ((buffer[12 + nth]) | (buffer[12 + nth + 1] >> 8));
  }
  
  nth += 2;

  /* software revision level */
  if(software_revision_level != NULL){
    device_family_number[0] = (buffer[12 + nth]) | (buffer[12 + nth + 1]) | (buffer[12 + nth + 2]) | (buffer[12 + nth + 3]);
  }

  nth += 4;
  
  /* capability */
  buffer[12 + nth] = capability;
  nth++;

  /* maximum sysex message size */
  if(max_sysex_message_size != NULL){
    max_sysex_message_size[0] = (buffer[12 + nth]) | (buffer[12 + nth + 1]) | (buffer[12 + nth + 2]) | (buffer[12 + nth + 3]);
  }

  nth += 4;
  
  buffer[12 + nth] = 0xf7;
  nth++;
    
  return(12 + nth);
}
