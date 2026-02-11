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

unsigned int major_format;

char *wav_filename = "out.wav";

int samplerate = 44100;
int pcm_channels = 1;
int audio_channels = 2;
int buffer_size = 2048;

SF_INFO *info = NULL;
SNDFILE *file = NULL;

float stereo_buffer[4096];

int
hw_input_callback(unsigned int in_device, const struct AudioTimeStamp *in_now, const struct AudioBufferList *in_input_data, const struct AudioTimeStamp *in_input_time, struct AudioBufferList *out_output_data, const struct AudioTimeStamp *in_output_time, void *in_client_data)
{
  AudioBuffer *in_buffer;
  
  float *buffer;

  int i;
  
  in_buffer = in_input_data->mBuffers;
  
  buffer = in_buffer->mData;

  for(i = 0; i < buffer_size; i++){
    stereo_buffer[2 * i] = buffer[i];
    stereo_buffer[2 * i + 1] = buffer[i];
  }
  
  sf_write_float(file, stereo_buffer, 2 * (in_buffer->mDataByteSize / sizeof(float)));
  
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
  int duration;
  int input_buffer_size_bytes;
  
  UInt32 property_size;
  
  OSStatus os_status;

  duration = 10;

  if(argc > 1){
    duration = strtoull(argv[1],
			NULL,
			10);
  }
  
  info = (SF_INFO *) malloc(sizeof(SF_INFO));

  info[0] = (SF_INFO) {0,};

  info->samplerate = samplerate;
  info->channels = audio_channels;

  major_format = SF_FORMAT_WAV;
    
  info->format = major_format | SF_FORMAT_FLOAT;

  info->frames = 0;
  info->seekable = 0;
  info->sections = 0;

  if(!sf_format_check(info)){
    fprintf(stderr, "invalid format\n");
  }
  
  file = (SNDFILE *) sf_open(wav_filename, SFM_RDWR, info);
      
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
      
  input_samplerate = (Float64) samplerate;
      
  AudioObjectSetPropertyData(input_device,
			     &input_samplerate_property_address,
			     0,
			     NULL,
			     sizeof(input_samplerate),
			     &input_samplerate);

  input_buffer_size_bytes = pcm_channels * buffer_size * sizeof(float);

  AudioObjectSetPropertyData(input_device,
			     &input_buffer_size_property_address,
			     0,
			     NULL,
			     sizeof(input_buffer_size_bytes),
			     &input_buffer_size_bytes);
      
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
  
  while(current_time.tv_sec > start_time.tv_sec + duration){
    clock_get_time(cclock, &current_time);

    usleep(5000000);
  }
  
  sf_close(file);
  
  mach_port_deallocate(mach_task_self(), cclock);

  return(0);
}
