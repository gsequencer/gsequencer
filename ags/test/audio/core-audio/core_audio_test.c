#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

int
main(int argc, char **argv)
{
  AudioDeviceID *audio_devices;
  
  NSString *device_uid = @"";

  AudioObjectPropertyAddress devices_property_address;
  AudioObjectPropertyAddress streams_property_address;

  int device_count;
  int stream_count;
  int is_mic;
  UInt32 prop_size;
  int i;
  OSStatus error;
  
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMaster;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeInput;

  error = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size);
  
  if(error == noErr){
    device_count = prop_size / sizeof(AudioDeviceID);
    
    audio_devices = (AudioDeviceID *) malloc(prop_size);
    
    error = AudioObjectGetPropertyData(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size, audio_devices);
    
    if(error == noErr) {
      for(i = 1; i <= device_count; i++){
	NSString *current_manufacturer, *current_name, *current_uid;
	
	prop_size = sizeof(CFStringRef);
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_manufacturer);
	
	if(error != noErr){
	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceNameCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_name);
	
	if(error != noErr){
	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceUID;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_uid);
	
	if(error != noErr){
	  continue;
	}
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Output"]){
	  device_uid = current_uid;
	}

	is_mic = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_mic = YES;
	}

	printf("found %s device: %s - %s <%s>\n", (!is_mic ? "output": "input"),  [current_manufacturer UTF8String], [current_name UTF8String], [current_uid UTF8String]);
      }
    }
    
    free(audio_devices);
  }
  
  return(0);
}
