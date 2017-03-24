//
//  VideoDecoder.h
//  WhisperDemo
//
//  Created by suleyu on 2017/3/23.
//  Copyright © 2017年 Kortide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <VideoToolbox/VideoToolbox.h>

@protocol VideoDecoderDelegate;

@interface VideoDecoder : NSObject

- (void)decode:(NSData *)data;
- (void)end;

@property (weak, nonatomic) id<VideoDecoderDelegate> delegate;

@end

@protocol VideoDecoderDelegate

- (void)videoDecoder:(VideoDecoder *)encoder gotSampleBuffer:(CMSampleBufferRef)sampleBuffer;
- (void)videoDecoder:(VideoDecoder *)encoder error:(NSString *)error;

@end
