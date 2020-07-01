//
//  main.m
//  AM2Discord
//
//  Created by Peter Carenzo on 5/12/20.
//

#import <Cocoa/Cocoa.h>
#import "ipc.h"
#define kAlreadyBeenLaunched @"AlreadyBeenLaunched"

//creates the compilied script which is used in the next section.
NSAppleScript *ScriptCreation()
{
    
    NSURL *scriptURL = [[NSURL alloc] initFileURLWithPath:[[NSBundle mainBundle] pathForResource:@"AM2Discord" ofType:@"scpt"]];
    
    NSDictionary * appleScriptCreationError = nil;
    NSAppleScript *script = [[NSAppleScript alloc] initWithContentsOfURL:scriptURL error:&appleScriptCreationError];
    
    if (appleScriptCreationError)
    {
        NSLog(@"%@", [NSString stringWithFormat:@"Could not instantiate applescript %@",appleScriptCreationError]);
    }
    
    return script;
    
}

/*
main update acivity call, queries the song information withe apple script
and then passes this updateactivity function in ipc.c if there is an execution 
error with the script it will sleep for 10 seconds and each consecutive script
error will cause the sleep time to be doubled by 2 up to 120 seconds at which point
it will just continually sleep 120 seconds. I did this to prevent instances where like 
itunes is open but not playing anything yet and so it doesnt keep querying alot

Also when it is working properly there is a 10 second waiting time between each activityUpdate
call this is to keep it from just bogging down systems with constant updates which arent really necessary
so if you change a song and don't see an immediate update then its most likely cause its still waiting 
that 10 seconds to update.
*/
void updateActivityObjc(NSAppleScript *script)
{
    int sleepTime = 10;
    while (true)
    {
        struct songInfo song;

        NSDictionary * executionError = nil;
        NSAppleEventDescriptor *theResult = [script executeAndReturnError:&executionError];
        if (executionError != nil)
        {
            NSLog(@"%@", [NSString stringWithFormat:@"error while executing script. Error %@",executionError]);
            
            struct songInfo emptySong;
            emptySong.songName = "    ";
            emptySong.artistName = "    ";
            emptySong.albumName = "    ";
            emptySong.state = "NONE";
            emptySong.startTime = 0;
            
            mainFunction(emptySong);
            
            sleep(sleepTime);
            
            if (sleepTime != 120)
            {
                sleepTime *= 2;
            }
            

        } else {
            
            NSArray *arrayOfComponents = [theResult.stringValue componentsSeparatedByString:@";;"];
            
            //Music
            song.songName = (const char *) [arrayOfComponents[0] UTF8String];
            song.artistName = (const char *) [arrayOfComponents[1] UTF8String];
            song.albumName = (const char *) [arrayOfComponents[2] UTF8String];
            song.state = (const char *) [arrayOfComponents[3] UTF8String];
            NSNumber *start = arrayOfComponents[4];
            song.startTime = start.intValue;
            
            mainFunction(song);
            sleep(10);
            sleepTime = 10;
        }
    }
}


int main(int argc, const char * argv[]) {
    @autoreleasepool {
    
    }
    

    if (startConnection() == -1) {
        [[NSApplication sharedApplication] terminate:nil];
    }

    NSAppleScript *script = ScriptCreation();
    
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_LOW, 0), ^{
        updateActivityObjc(script);
    });


    
    return NSApplicationMain(argc, argv);
}
