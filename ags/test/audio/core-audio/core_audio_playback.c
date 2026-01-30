#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

#include <math.h>

#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <stdio.h>

#include <mach/clock.h>
#include <mach/mach.h>

int samplerate = 44100;
int pcm_channels = 1;
int audio_channels = 2;
int buffer_size = 2048;

int offset = 0;

int
hw_output_callback(AudioObjectID in_device, const AudioTimeStamp *in_now, const AudioBufferList *in_input_data, const AudioTimeStamp *in_input_time, AudioBufferList *out_output_data, const AudioTimeStamp *in_output_time, void *in_client_data)
{
  AudioBuffer *out_buffer;
  
  Float32 *buffer;

  int i;
  
  out_buffer = out_output_data->mBuffers;
  
  buffer = out_buffer->mData;

  for(i = 0; i < buffer_size; i++){
    buffer[audio_channels * i] = 
      buffer[audio_channels * i + 1] = sin((double) (offset + i) * 2.0 * M_PI * 440.0 / (double) samplerate);
  }

  offset += i;
  
  return(0);
}

int
main(int argc, char **argv)
{
  AudioDeviceID *audio_devices;
  AudioDeviceID output_device;
  
  NSString *device_uid = @"";

  AudioDeviceIOProcID output_proc_id;
  
  AudioObjectPropertyAddress output_property_address;
  AudioObjectPropertyAddress output_samplerate_property_address;
  AudioObjectPropertyAddress output_buffer_size_property_address;
  
  AudioObjectPropertyAddress devices_property_address;
  AudioObjectPropertyAddress streams_property_address;
  
  struct AudioStreamBasicDescription stream_desc;
  
  clock_serv_t cclock;
  mach_timespec_t start_time, current_time;

  char *card_name;
  char *str;
  
  Float64 output_samplerate;
  int duration;
  int output_buffer_size_bytes;
  
  int device_count;
  int stream_count;
  int is_speaker;
  int is_mic;
  UInt32 prop_size;
  
  UInt32 property_size;

  int i;
  
  OSStatus os_status;

  duration = 10;

  card_name = NULL;
  
  if(argc > 1){
    duration = strtoull(argv[1],
			NULL,
			10);
  }

  if(argc > 2){
    card_name = argv[2];
  }
  
  output_device = 0;
  
  output_property_address.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
  output_property_address.mElement = kAudioObjectPropertyElementMain;
  output_property_address.mScope = kAudioObjectPropertyScopeGlobal;

  output_samplerate_property_address.mSelector = kAudioDevicePropertyNominalSampleRate;
  output_samplerate_property_address.mElement = kAudioObjectPropertyElementMain;
  output_samplerate_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  
  output_buffer_size_property_address.mSelector = kAudioDevicePropertyBufferSize;
  output_buffer_size_property_address.mElement = kAudioObjectPropertyElementMain;
  output_buffer_size_property_address.mScope = kAudioObjectPropertyScopeGlobal;

  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMain;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeOutput;
  
  if(argc > 2){
    os_status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size);
    
    if(os_status == noErr){
      device_count = prop_size / sizeof(AudioDeviceID);
    
      audio_devices = (AudioDeviceID *) malloc(prop_size);
    
      os_status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size, audio_devices);
    
      if(os_status == noErr) {
	for(i = 0; i < device_count; i++){
	  NSString *current_manufacturer, *current_name, *current_uid;
	
	  prop_size = sizeof(CFStringRef);
	
	  devices_property_address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
	  os_status = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_manufacturer);
	
	  if(os_status != noErr){
	    current_manufacturer = @"";
	  
	    //	  continue;
	  }
	
	  devices_property_address.mSelector = kAudioDevicePropertyDeviceNameCFString;
	  os_status = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_name);
	
	  if(os_status != noErr){
	    current_name = @"";

	    //	  continue;
	  }
	
	  devices_property_address.mSelector = kAudioDevicePropertyDeviceUID;
	  os_status = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_uid);
	
	  if(os_status != noErr){
	    current_uid = @"";
	  
	    //	  continue;
	  }
	
	  if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Output"]){
	    device_uid = current_uid;
	  }

	  is_speaker = 0;

	  os_status = AudioObjectGetPropertyDataSize(audio_devices[i], 
						     &streams_property_address, 
						     0, 
						     NULL, 
						     &prop_size);
	
	  stream_count = prop_size / sizeof(AudioStreamID);

	  if(stream_count > 0){
	    is_speaker = YES;
	  }

	  str = malloc(2048 * sizeof(char));
	  memset(str, 0, 2048 * sizeof(char));

	  fprintf(stderr, "%s - %s\n", [current_manufacturer UTF8String], [current_name UTF8String]);
	
	  sprintf(str, "%s - %s", [current_manufacturer UTF8String], [current_name UTF8String]);
	
	  if(is_speaker &&
	     !strcmp(str, card_name)){
	    fprintf(stderr, " `- success\n");
	  
	    output_device = audio_devices[i];

	    free(str);
	  
	    break;
	  }

	  free(str);
	}
      }
    
      free(audio_devices);
    }
  }else{
    output_device = 0;
  
    AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,
      &output_property_address,
      0,
      NULL,
      &property_size);
	
    AudioObjectGetPropertyData(kAudioObjectSystemObject, 
      &output_property_address,
      0, 
      NULL, 
      &property_size, 
      &(output_device));
  }
    
  output_samplerate = (Float64) samplerate;
      
  AudioObjectSetPropertyData(output_device,
			     &output_samplerate_property_address,
			     0,
			     NULL,
			     sizeof(output_samplerate),
			     &output_samplerate);

  output_buffer_size_bytes = pcm_channels * buffer_size * sizeof(float);

  AudioObjectSetPropertyData(output_device,
			     &output_buffer_size_property_address,
			     0,
			     NULL,
			     sizeof(output_buffer_size_bytes),
			     &output_buffer_size_bytes);
      
  os_status = AudioDeviceCreateIOProcID(output_device,
			    (OSStatus (*)(AudioObjectID inDevice, const AudioTimeStamp *inNow, const AudioBufferList *inInputData, const AudioTimeStamp *inInputTime, AudioBufferList *outOutputData, const AudioTimeStamp *inOutputTime, void *inClientData)) hw_output_callback,
			    NULL,
			    &(output_proc_id));

  if(os_status != noErr){
    fprintf(stderr, "failed create IOProcID\n");
  }
  
  AudioDeviceStart(output_device,
		   output_proc_id);
  
  host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);

  clock_get_time(cclock, &start_time);

  current_time.tv_sec = 0;
  current_time.tv_nsec = 0;
  
  while(current_time.tv_sec < start_time.tv_sec + duration){
    clock_get_time(cclock, &current_time);

    usleep(5000000);
  }
  
  mach_port_deallocate(mach_task_self(), cclock);

  return(0);
}
