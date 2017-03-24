//
//  VideoEncoder.m
//  WhisperDemo
//
//  Created by suleyu on 2017/3/23.
//  Copyright © 2017年 Kortide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VideoDecoder.h"
#include "CRtpUnpack.h"

@implementation VideoDecoder
{
    dispatch_queue_t queue;
    CRtpUnpack *rtpUnpack;
    NSData *spsData;
    NSData *ppsData;
    CMVideoFormatDescriptionRef videoFormatDescription;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        // Custom initialization
        queue = dispatch_queue_create("videoDecoder", NULL);
    }
    return self;
}

- (void)dealloc
{
    [self end];
    queue = NULL;
}

- (void)decode:(NSData *)data
{
    dispatch_async(queue, ^{
        
        if (rtpUnpack == NULL) {
            int initError;
            rtpUnpack = new CRtpUnpack(initError);
            if (initError != 0) {
                delete rtpUnpack;
                rtpUnpack = NULL;
                
                NSLog(@"RTP: Create CRtpUnpack error: %d", initError);
                [self.delegate videoDecoder:self error:@"Create CRtpUnpack failed"];
                return;
            }
        }
        
        unsigned short rtpLength = data.length;
        unsigned char *pRtpData = new unsigned char[rtpLength];
        [data getBytes:pRtpData length:rtpLength];
        
        unsigned int frameLength = 0;
        unsigned int timestamp = 0;
        unsigned char *pFrameData = rtpUnpack->Parse_RTP_Packet(pRtpData, rtpLength, &frameLength, &timestamp);
        if (pFrameData != NULL && frameLength > 4)
        {
            [self hardwareDecodeFrameData:pFrameData length:frameLength];
        }
        
        delete [] pRtpData;
        pRtpData = NULL;
    });
}

- (void)hardwareDecodeFrameData:(unsigned char*)pFrameData length:(unsigned int)frameLength
{
    int naluType = ((uint8_t)pFrameData[4] & 0x1F);
    //NSLog(@"RTP: timestamp: %d, nalu header: %x, type: %d, size: %d", timestamp, pFrameData[4], naluType, length);
    
    if ((naluType == 7 || naluType == 8) && videoFormatDescription == NULL) {
        if (naluType == 7) {
            spsData = [NSData dataWithBytes:pFrameData + 4 length:frameLength - 4];
        }
        
        if (naluType == 8) {
            ppsData = [NSData dataWithBytes:pFrameData + 4 length:frameLength - 4];
        }
        
        if (spsData != nil && ppsData != nil) {
            const uint8_t* const parameterSetPointers[2] = { (const uint8_t*)[spsData bytes], (const uint8_t*)[ppsData bytes] };
            const size_t parameterSetSizes[2] = { spsData.length, ppsData.length };
            
            //construct h.264 parameter set
            CMVideoFormatDescriptionRef formatDesc = NULL;
            OSStatus formatCreateResult = CMVideoFormatDescriptionCreateFromH264ParameterSets(kCFAllocatorDefault, 2, parameterSetPointers, parameterSetSizes, 4, &formatDesc);
            if (formatCreateResult == noErr) {
                videoFormatDescription = formatDesc;
            }
            else {
                NSLog(@"H264 decode: CMVideoFormatDescriptionCreateFromH264ParameterSets error : %d", (int)formatCreateResult);
                [self end];
                [self.delegate videoDecoder:self error:@"Create video format description failed"];
            }
        }
    }
    else if ((naluType == 1 || naluType == 5) && videoFormatDescription) {
        uint32_t dataLength32 = htonl(frameLength - 4);
        memcpy (pFrameData, &dataLength32, sizeof(uint32_t));
        
        CMBlockBufferRef blockBuffer = NULL;
//        OSStatus status = CMBlockBufferCreateEmpty(NULL, 0, kCMBlockBufferAlwaysCopyDataFlag, &blockBuffer);
//        if (status == kCMBlockBufferNoErr) {
//            status = CMBlockBufferAppendMemoryBlock(blockBuffer,
//                                                    pFrameData,
//                                                    frameLength,
//                                                    NULL,
//                                                    NULL,
//                                                    0,
//                                                    frameLength,
//                                                    kCMBlockBufferAlwaysCopyDataFlag);
//        }
//        else {
//            DLogError(@"Create empty block buffer failed : %d", status);
//        }
        OSStatus status = CMBlockBufferCreateWithMemoryBlock(NULL, pFrameData, frameLength, kCFAllocatorNull, NULL, 0, frameLength, 0, &blockBuffer);
        if (status == kCMBlockBufferNoErr) {
            const size_t sampleSize = frameLength; // CMBlockBufferGetDataLength(blockBuffer);
            CMSampleBufferRef sampleBuffer = NULL;
            status = CMSampleBufferCreateReady(kCFAllocatorDefault,
                                               blockBuffer,
                                               videoFormatDescription,
                                               1,
                                               0,
                                               NULL,
                                               1,
                                               &sampleSize,
                                               &sampleBuffer);
            if (status == noErr) {
                // set some values of the sample buffer's attachments
                CFArrayRef attachments = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, YES);
                CFMutableDictionaryRef dict = (CFMutableDictionaryRef)CFArrayGetValueAtIndex(attachments, 0);
                CFDictionarySetValue(dict, kCMSampleAttachmentKey_DisplayImmediately, kCFBooleanTrue);
                
                [self.delegate videoDecoder:self gotSampleBuffer:sampleBuffer];
                
                CFRelease(sampleBuffer);
            }
            else {
                NSLog(@"H264 decode: CMSampleBufferCreate error : %d", (int)status);
                [self end];
                [self.delegate videoDecoder:self error:@"Create sample buffer failed"];
            }
            
            CFRelease(blockBuffer);
        }
        else {
            NSLog(@"H264 decode: CMBlockBufferCreateWithMemoryBlock error : %d", (int)status);
            [self end];
            [self.delegate videoDecoder:self error:@"Create block buffer failed"];
        }
    }
}

- (void)end
{
    if (rtpUnpack) {
        delete rtpUnpack;
        rtpUnpack = NULL;
    }
    
    if (videoFormatDescription) {
        CFRelease(videoFormatDescription);
        videoFormatDescription = NULL;
    }
    
    spsData = nil;
    ppsData = nil;
}

@end
