//
//  ViewController.m
//  Harmonia_TestApp
//
//  Created by Hiiragi on 2017/11/23.
//  Copyright © 2017年 Hiiragi. All rights reserved.
//

#import "ViewController.h"
#import "HarmoniaInterface.h"

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    NSString *fileName = [[NSBundle mainBundle] pathForResource:@"test" ofType:@"ogg"];
    NSURL *url = [NSURL fileURLWithPath:fileName];
    NSData *data = [[NSData alloc] initWithContentsOfURL:url];
    
    if (data != nil)
    {
        unsigned char* buffer = (unsigned char *)[data bytes];
        int size = (int)[data length];
        
        harmonia_initialize(2048);
        
        harmonia_register_sound("bgm1", buffer, size, 269128, 2116816);
        
        harmonia_sound_play("bgm1");
    }
    else
    {
        printf("file is not exists.");
    }
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
