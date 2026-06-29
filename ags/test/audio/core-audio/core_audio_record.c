/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

#include <sndfile.h>

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <mach/clock.h>
#include <mach/mach.h>

#define CORE_AUDIO_RECORD_PCM_BUFFER_SIZE (4096)
#define CORE_AUDIO_RECORD_BUFFER_SIZE (2048)

struct _CoreAudioRecord
{
  unsigned int major_format;

  char *wav_filename;

  int samplerate;
  int pcm_channels;
  int audio_channels;
  int buffer_size;

  SF_INFO *info;
  SNDFILE *file;

  float stereo_buffer[CORE_AUDIO_RECORD_PCM_BUFFER_SIZE];
};

struct _CoreAudioRecord record = {0,};

int
hw_input_callback(AudioObjectID in_device, const struct AudioTimeStamp *in_now, const struct AudioBufferList *in_input_data, const struct AudioTimeStamp *in_input_time, struct AudioBufferList *out_output_data, const struct AudioTimeStamp *in_output_time, void *in_client_data)
{
  AudioBuffer *in_buffer;
  
  float *buffer;

  int i;
  
  in_buffer = in_input_data->mBuffers;
  
  buffer = in_buffer->mData;

  for(i = 0; i < record.buffer_size && 2 * i < CORE_AUDIO_RECORD_PCM_BUFFER_SIZE && i < in_buffer->mDataByteSize / sizeof(float); i++){
    record.stereo_buffer[2 * i] = buffer[i];
    record.stereo_buffer[2 * i + 1] = buffer[i];
  }
  
  sf_write_float(record.file, record.stereo_buffer, CORE_AUDIO_RECORD_PCM_BUFFER_SIZE);
  
  return(0);
}

int
main(int argc, char **argv)
{
  AudioDeviceID input_device;

  AudioDeviceIOProcID input_proc_id;
  
  AudioObjectPropertyAddress input_property_address;
  AudioObjectPropertyAddress input_samplerate_property_address;
  AudioObjectPropertyAddress input_buffer_size_property_address;

  clock_serv_t cclock;
  mach_timespec_t start_time, current_time;
  
  Float64 input_samplerate;
  Int64 duration;
  int input_buffer_size_bytes;
  
  UInt32 property_size;
  
  OSStatus os_status;

  duration = 10;

  if(argc > 1){
    duration = strtoull(argv[1],
			NULL,
			10);
  }
  
  record.wav_filename = "out.wav";

  record.samplerate = 44100;
  record.pcm_channels = 1;
  record.audio_channels = 2;
  record.buffer_size = CORE_AUDIO_RECORD_BUFFER_SIZE;

  record.info = NULL;
  record.file = NULL;
  
  record.info = (SF_INFO *) malloc(sizeof(SF_INFO));

  record.info[0] = (SF_INFO) {0,};

  record.info->samplerate = record.samplerate;
  record.info->channels = record.audio_channels;

  record.major_format = SF_FORMAT_WAV;
    
  record.info->format = record.major_format | SF_FORMAT_FLOAT;

  record.info->frames = 0;
  record.info->seekable = 0;
  record.info->sections = 0;

  if(!sf_format_check(record.info)){
    fprintf(stderr, "invalid format\n");
  }
  
  record.file = (SNDFILE *) sf_open(record.wav_filename, SFM_RDWR, record.info);
      
  input_property_address.mSelector = kAudioHardwarePropertyDefaultInputDevice;
  input_property_address.mElement = kAudioObjectPropertyElementMain;
  input_property_address.mScope = kAudioObjectPropertyScopeGlobal;

  input_samplerate_property_address.mSelector = kAudioDevicePropertyNominalSampleRate;
  input_samplerate_property_address.mElement = kAudioObjectPropertyElementMain;
  input_samplerate_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  
  input_buffer_size_property_address.mSelector = kAudioDevicePropertyBufferSize;
  input_buffer_size_property_address.mElement = kAudioObjectPropertyElementMain;
  input_buffer_size_property_address.mScope = kAudioObjectPropertyScopeGlobal;

  input_device = 0;
  
  AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
				 &input_property_address,
				 0,
				 NULL,
				 &property_size);
	
  AudioObjectGetPropertyData(kAudioObjectSystemObject, 
			     &input_property_address,
			     0, 
			     NULL, 
			     &property_size, 
			     &(input_device));
      
  input_samplerate = (Float64) record.samplerate;
      
  AudioObjectSetPropertyData(input_device,
			     &input_samplerate_property_address,
			     0,
			     NULL,
			     sizeof(input_samplerate),
			     &input_samplerate);

  input_buffer_size_bytes = record.pcm_channels * record.buffer_size * (int) sizeof(float);

  AudioObjectSetPropertyData(input_device,
			     &input_buffer_size_property_address,
			     0,
			     NULL,
			     sizeof(input_buffer_size_bytes),
			     (void *) &input_buffer_size_bytes);
      
  AudioDeviceCreateIOProcID(input_device,
			    (OSStatus (*)(AudioObjectID inDevice, const AudioTimeStamp *inNow, const AudioBufferList *inInputData, const AudioTimeStamp *inInputTime, AudioBufferList *outOutputData, const AudioTimeStamp *inOutputTime, void *inClientData)) hw_input_callback,
			    NULL,
			    &(input_proc_id));

  AudioDeviceStart(input_device,
		   input_proc_id);
  
  host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);

  clock_get_time(cclock, &start_time);

  current_time.tv_sec = 0;
  current_time.tv_nsec = 0;
  
  while(current_time.tv_sec < start_time.tv_sec + duration){
    clock_get_time(cclock, &current_time);

    usleep(5000000);
  }
  
  sf_close(record.file);
  
  mach_port_deallocate(mach_task_self(), cclock);

  return(0);
}
