/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_WAV_FILE_H__
#define __AGS_WAV_FILE_H__

#include <glib.h>
#include <glib-object.h>

#include <stdio.h>

#define AGS_TYPE_WAV_FILE                (ags_wav_file_get_type())
#define AGS_WAV_FILE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_WAV_FILE, AgsWavFile))
#define AGS_WAV_FILE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_WAV_FILE, AgsWavFileClass))
#define AGS_IS_WAV_FILE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_WAV_FILE))
#define AGS_IS_WAV_FILE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_WAV_FILE))
#define AGS_WAV_FILE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_WAV_FILE, AgsWavFileClass))

#define AGS_WAV_FILE_CHUNK(ptr) ((AgsWavFileChunk *)(ptr))

#define AGS_WAV_FILE_RIFF "RIFF"
#define AGS_WAV_FILE_RIFX "RIFX"
#define AGS_WAV_FILE_RF64 "RF64"

#define AGS_WAV_FILE_WAVE "WAVE"

#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_SIZE (16)
#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_FORMAT (AGS_WAV_FILE_FORMAT_PCM)
#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_CHANNELS (2)
#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_SAMPLERATE (44100)
#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_BPS (AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_CHANNELS * \
					       AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_SAMPLERATE * \
					       2)
#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_BLOCK_ALIGN (2)
#define AGS_WAV_FILE_DEFAULT_FORMAT_CHUNK_BITS_PER_SAMPLE (16)

#define AGS_WAV_FILE_DEFAULT_FACT_CHUNK_SIZE (4)

typedef struct _AgsWavFile AgsWavFile;
typedef struct _AgsWavFileClass AgsWavFileClass;
typedef struct _AgsWavFileChunk AgsWavFileChunk;
typedef struct _AgsWavFileFormatChunk AgsWavFileFormatChunk;
typedef struct _AgsWavFileFactChunk AgsWavFileFactChunk;
typedef struct _AgsWavFileDataChunk AgsWavFileDataChunk;

typedef enum{
  AGS_WAV_FILE_READ_ONLY  =  1,
}AgsWavFileFlags;

typedef enum{
  AGS_WAV_FILE_FORMAT_UNKNOWN           = 0x0000,
  AGS_WAV_FILE_FORMAT_PCM               = 0x0001,
  AGS_WAV_FILE_FORMAT_IEEE_FLOAT        = 0x0003,
  AGS_WAV_FILE_FORMAT_ALAW              = 0x0006,
  AGS_WAV_FILE_FORMAT_MULAW             = 0x0007,
  AGS_WAV_FILE_FORMAT_OKI_ADPCM         = 0x0010,
  AGS_WAV_FILE_FORMAT_IMA_ADPCM         = 0x0011,
  AGS_WAV_FILE_FORMAT_DIGISTD           = 0x0015,
  AGS_WAV_FILE_FORMAT_DIGIFIX           = 0x0016,
  AGS_WAV_FILE_FORMAT_DOLBY_AC2         = 0x0030,
  AGS_WAV_FILE_FORMAT_GSM610            = 0x0031,
  AGS_WAV_FILE_FORMAT_ROCKWELL_ADPCM    = 0x003b,
  AGS_WAV_FILE_FORMAT_ROCKWELL_DIGITALK = 0x003c,
  AGS_WAV_FILE_FORMAT_G721_ADPCM        = 0x0040,
  AGS_WAV_FILE_FORMAT_G728_CELP         = 0x0041,
  AGS_WAV_FILE_FORMAT_MPEG              = 0x0050,
  AGS_WAV_FILE_FORMAT_MPEGLAYER3        = 0x0055,
  AGS_WAV_FILE_FORMAT_G726_ADPCM        = 0x0064,
  AGS_WAV_FILE_FORMAT_G722_ADPCM        = 0x0065,
  AGS_WAV_FILE_FORMAT_EXTENSIBLE        = 0xfffe,
}AgsWavFileFormat;

struct _AgsWavFile
{
  GObject object;

  guint flags;
  
  gchar *filename;
  
  FILE *stream;
  
  gchar chunk_id[4];
  guint32 chunk_size;

  gchar wave_id[4];

  AgsWavFileFormatChunk *current_format_chunk;
  AgsWavFileFactChunk *current_fact_chunk;
  AgsWavFileDataChunk *current_data_chunk;
  
  guint64 offset;
  
  GList *format_chunk;
  GList *fact_chunk;
  GList *data_chunk;
  
  GList *all_chunk;
};

struct _AgsWavFileClass
{
  GObjectClass object;
};

struct _AgsWavFileChunk
{
  gchar chunk_id[4];
  guint32 chunk_size;
};

struct _AgsWavFileFormatChunk
{
  AgsWavFileChunk chunk;
  
  guint32 format;
  guint16 channels;
  guint32 samplerate;
  guint32 bps;
  guint16 block_align;
  guint16 bits_per_sample;
  guint16 cb_size;
  guint16 valid_bits_per_sample;
  guint32 channel_mask;
  gchar guid[16];
};

struct _AgsWavFileFactChunk
{
  AgsWavFileChunk chunk;

  guint32 samples_per_channel;
};

struct _AgsWavFileDataChunk
{
  AgsWavFileChunk chunk;

  void *data;
};

GType ags_wav_file_get_type();

AgsWavFileFormatChunk* ags_wav_file_format_chunk_alloc();
void ags_wav_file_format_chunk_free(AgsWavFileFormatChunk *format_chunk);

AgsWavFileFactChunk* ags_wav_file_fact_chunk_alloc();
void ags_wav_file_fact_chunk_free(AgsWavFileFactChunk *fact_chunk);

AgsWavFileDataChunk* ags_wav_file_data_chunk_alloc();
void ags_wav_file_data_chunk_free(AgsWavFileDataChunk *data_chunk);

void ags_wav_file_add_format_chunk(AgsWavFile *wav_file,
				   AgsWavFileFormatChunk *format_chunk);
void ags_wav_file_remove_format_chunk(AgsWavFile *wav_file,
				      AgsWavFileFormatChunk *format_chunk);

void ags_wav_file_add_fact_chunk(AgsWavFile *wav_file,
				 AgsWavFileFactChunk *fact_chunk);
void ags_wav_file_remove_fact_chunk(AgsWavFile *wav_file,
				    AgsWavFileFactChunk *fact_chunk);

void ags_wav_file_add_data_chunk(AgsWavFile *wav_file,
				 AgsWavFileDataChunk *data_chunk);
void ags_wav_file_remove_data_chunk(AgsWavFile *wav_file,
				    AgsWavFileDataChunk *data_chunk);

gchar** ags_wav_file_get_valid_suffix();

gboolean ags_wav_file_check_open(gchar *filename);

AgsWavFile* ags_wav_file_new(gchar *filename);

#endif /*__AGS_WAV_FILE_H__*/
