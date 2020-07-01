//
//  AppDelegate.m
//  AM2Discord
//
//  Created by Peter Carenzo on 5/12/20.
//

#import "AppDelegate.h"

@interface AppDelegate ()


@property NSStatusItem *barItem;
@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    self.barItem = [NSStatusBar.systemStatusBar statusItemWithLength:NSSquareStatusItemLength];
    NSImage *icon = [NSImage imageNamed:@"icon"];
    icon.template = YES;
    self.barItem.button.image = icon;
    
    [self constructMenu];
    

}

- (void)quitAction {
    [[NSApplication sharedApplication] terminate:self];
}

- (void)constructMenu {
    NSMenu *menu = [[NSMenu alloc]initWithTitle:@""];
    NSMenuItem *quitItem = [[NSMenuItem alloc]initWithTitle:@"Quit" action:@selector(quitAction) keyEquivalent:@""];

    [menu addItem:quitItem];
    
    self.barItem.menu = menu;
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    disconnect();
}


@end
