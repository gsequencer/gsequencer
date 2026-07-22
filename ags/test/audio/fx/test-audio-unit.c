#include <CoreFoundation/CoreFoundation.h>
#include <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/AUComponent.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <CoreAudio/CoreAudio.h>

#include <mach/mach_time.h>

void
ProcessFoundAudioComponent(AudioComponent theComponent)
{
  CFStringRef compName;
  
  AudioComponentCopyName(theComponent, &compName);
  
  NSLog(@"%@", compName);
  
  CFRelease(compName);
}

void
EnumerateSandboxSafeAudioComponents()
{
    //	make a description that includes all wildcards except for the flags and flags
    //	mask so that we traverse all the AudioComponents that are sandbox safe
    AudioComponent theComponent;
    AudioComponentDescription theDescription;
    
    memset(&theDescription, 0, sizeof(theDescription));
 
    //	Use the flag to indicate that we want to find Sandbox Safe AudioComponents
    theDescription.componentFlags = kAudioComponentFlag_SandboxSafe;
    theDescription.componentFlagsMask = kAudioComponentFlag_SandboxSafe;
 
    //	get the first AudioComponent
    theComponent = AudioComponentFindNext(NULL, &theDescription);
    
    while(theComponent != NULL)
    {
        //	process the AudioComponent
        ProcessFoundAudioComponent(theComponent);
 
        //	get the next one in the list
        theComponent = AudioComponentFindNext(theComponent, &theDescription);
    }
}

int
main()
{
  EnumerateSandboxSafeAudioComponents();
  
  return(0);
}
