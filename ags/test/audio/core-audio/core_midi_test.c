#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>

#include <stdio.h>

int
main(int argc, char **argv)
{
  MIDIObjectRef endpoint;

  CFStringRef current_uid;
  CFStringRef current_manufacturer;
  CFStringRef current_name;  
  
  ItemCount sources_count;

  int i;
  
  OSStatus error;

  sources_count = MIDIGetNumberOfSources();

  for(i = 0; i < sources_count; i++){
    endpoint = MIDIGetSource(i);

    if(endpoint != 0){
      current_uid = NULL;
      current_manufacturer = NULL;
      current_name = NULL;
      
      error = MIDIObjectGetStringProperty(endpoint, kMIDIPropertyUniqueID, &current_uid);

      if(error != noErr){
	current_uid = @"";
      }
      
      error = MIDIObjectGetStringProperty(endpoint, kMIDIPropertyManufacturer, &current_manufacturer);

      if(error != noErr){
	current_manufacturer = @"";
      }
      
      error = MIDIObjectGetStringProperty(endpoint, kMIDIPropertyName, &current_name);

      if(error != noErr){
	current_name = @"";
      }
      
      NSString *str_uid = (__bridge_transfer NSString *) current_uid;
      NSString *str_manufacturer = (__bridge_transfer NSString *) current_manufacturer;
      NSString *str_name = (__bridge_transfer NSString *) current_name;
      
      fprintf(stderr, "found device: %s - %s\n", [str_manufacturer UTF8String], [str_name UTF8String]);
      
      CFRelease(current_manufacturer);
      CFRelease(current_name);
    }
  }
  
  return(0);
}
