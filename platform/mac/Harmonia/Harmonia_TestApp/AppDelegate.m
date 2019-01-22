//
//  AppDelegate.m
//  Harmonia_TestApp
//
//  Created by Hiiragi on 2017/11/22.
//  Copyright © 2017年 Hiiragi. All rights reserved.
//

#include "HarmoniaInterface.h"
#import "AppDelegate.h"


@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    
    NSString *fileName = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"ogg"];
    NSURL *url = [NSURL fileURLWithPath:fileName];
    NSData *data = [[NSData alloc] initWithContentsOfURL:url];
    
    if (data != nil)
    {
        unsigned char* buffer = (unsigned char *)[data bytes];
        unsigned int size = (unsigned int)[data length];
        
        harmonia_initialize(2048);
        
        harmonia_register_sound("bgm1", buffer, size, 269128, 2116816);
        
        harmonia_sound_play("bgm1");
        
        /*
        Harmonia::initialize(2048);
        
        Harmonia::register_sound("bgm1", buffer, size);
        
        Harmonia::play("bgm1");
        */
    }
    else
    {
        printf("file is not exists.");
    }
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
