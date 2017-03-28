//
//  VideoEncoder.m
//  WhisperDemo
//
//  Created by suleyu on 2017/3/21.
//  Copyright © 2017年 Kortide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "VideoEncoder.h"
#include "CRtpStream.h"

static const int fps = 20;

@implementation VideoEncoder
{
    dispatch_queue_t queue;
    VTCompressionSessionRef encodingSession;
    CRtpStream *rtp;
}

- (instancetype)init
{
    self = [super init];
    if (self) {
        // Custom initialization
        queue = dispatch_queue_create("videoEncoder", NULL);
    }
    return self;
}

- (void)dealloc
{
    [self end];
    queue = NULL;
}

void didRtpStreamOut(void *callbackRefCon, const uint8_t *data, int length)
{
    VideoEncoder* encoder = (__bridge VideoEncoder*)callbackRefCon;
    [encoder->_delegate videoEncoder:encoder appendBytes:data length:length];
}

void didCompressH264(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags, CMSampleBufferRef sampleBuffer)
{
//    NSLog(@"didCompressH264 called with status %d infoFlags %d", (int)status, (int)infoFlags);
    if (status != noErr) {
        NSLog(@"H264 encode: encoding video error: %d", (int)status);
        return;
    }
    
    if (!CMSampleBufferDataIsReady(sampleBuffer)) {
        NSLog(@"H264 encode: didCompressH264 data is not ready");
        return;
    }
    
    NSMutableData *streamData = [NSMutableData data];
    VideoEncoder* encoder = (__bridge VideoEncoder*)outputCallbackRefCon;
    
    // This is the start code that we will write to
    // the elementary stream before every NAL unit
    static const size_t startCodeLength = 4;
    static const uint8_t startCode[] = {0x00, 0x00, 0x00, 0x01};
    
    // Find out if the sample buffer contains an I-Frame.
    // If so we will write the SPS and PPS NAL units to the elementary stream.
    //bool isIFrame = !CFDictionaryContainsKey( (CFArrayGetValueAtIndex(CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true), 0)), kCMSampleAttachmentKey_NotSync);
    BOOL isIFrame = NO;
    CFArrayRef attachmentsArray = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true);
    if (CFArrayGetCount(attachmentsArray)) {
        CFBooleanRef notSync;
        CFDictionaryRef dict = (CFDictionaryRef)CFArrayGetValueAtIndex(attachmentsArray, 0);
        BOOL keyExists = CFDictionaryGetValueIfPresent(dict, kCMSampleAttachmentKey_NotSync, (const void **)&notSync);
        // An I-Frame is a sync frame
        isIFrame = !keyExists || !CFBooleanGetValue(notSync);
    }
    
    if (isIFrame) {
        CMFormatDescriptionRef description = CMSampleBufferGetFormatDescription(sampleBuffer);
        
        // Find out how many parameter sets there are
        size_t parameterSetCount;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(description, 0, NULL, NULL, &parameterSetCount, NULL);
        if (statusCode == noErr) {
            // Write each parameter set to the elementary stream
            for (int i = 0; i < parameterSetCount; i++) {
                const uint8_t *parameterSetPointer;
                size_t parameterSetSize;
                OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(description, i, &parameterSetPointer, &parameterSetSize, NULL, NULL);
                if (statusCode == noErr) {
                    // Write the parameter set to the elementary stream
                    [streamData appendBytes:startCode length:startCodeLength];
                    [streamData appendBytes:parameterSetPointer length:parameterSetSize];
                }
            }
        }
    }
    
    CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t totalLength;
    uint8_t *dataPointer = NULL;
    OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, NULL, &totalLength, (char **)&dataPointer);
    if (statusCodeRet == noErr) {
        size_t bufferOffset = 0;
        static const int AVCCHeaderLength = 4;
        while (bufferOffset < totalLength - AVCCHeaderLength) {
            
            // Read the NAL unit length
            uint32_t NALUnitLength = 0;
            memcpy(&NALUnitLength, dataPointer + bufferOffset, AVCCHeaderLength);
            
            // Convert the length value from Big-endian to Little-endian
            NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
            
            // Write start code to the elementary stream
            [streamData appendBytes:startCode length:startCodeLength];
            // Write the NAL unit without the AVCC length header to the elementary stream
            [streamData appendBytes:dataPointer + bufferOffset + AVCCHeaderLength length:NALUnitLength];
            
            // Move to the next NAL unit in the block buffer
            bufferOffset += AVCCHeaderLength + NALUnitLength;
        }
    }
    
    if (streamData.length > 0) {
        CMTime presentationTimeStamp = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
        uint32_t timestamp = (uint32_t)(presentationTimeStamp.value * 1000 / presentationTimeStamp.timescale);
        encoder->rtp->streamOut((const uint8_t *)streamData.bytes, (int)streamData.length, timestamp);
    }
}

- (void)encode:(CMSampleBufferRef)sampleBuffer
{
    dispatch_sync(queue, ^{
        // Get the CV Image buffer
        CVImageBufferRef imageBuffer = (CVImageBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
        
        if (encodingSession == NULL) {
            CGSize size = CVImageBufferGetDisplaySize(imageBuffer);
            NSLog(@"Video width : %.0f, height : %.0f", size.width, size.height);
            
            // Create the compression session
            OSStatus status = VTCompressionSessionCreate(NULL, size.width, size.height, kCMVideoCodecType_H264, NULL, NULL, NULL, didCompressH264, (__bridge void *)(self),  &encodingSession);
            if (status != 0) {
                NSLog(@"H264 encode: VTCompressionSessionCreate error: %d", (int)status);
                [self.delegate videoEncoder:self error:@"Unable to create a H264 compression session"];
                return;
            }
            
            // Set the properties
            VTSessionSetProperty(encodingSession, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
            // ProfileLevel，h264的协议等级，不同的清晰度使用不同的ProfileLevel
            VTSessionSetProperty(encodingSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_Baseline_AutoLevel);
            // 关闭重排Frame，因为有了B帧（双向预测帧，根据前后的图像计算出本帧）后，编码顺序可能跟显示顺序不同
            VTSessionSetProperty(encodingSession, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
            // 视频帧率
            VTSessionSetProperty(encodingSession, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef)@(fps));
            // 关键帧最大间隔，1为每个都是关键帧，数值越大压缩率越高。此处表示关键帧最大间隔为1s
            VTSessionSetProperty(encodingSession, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef)@(fps));
            
            // Tell the encoder to start encoding
            VTCompressionSessionPrepareToEncodeFrames(encodingSession);
            
            rtp = new CRtpStream(didRtpStreamOut, (__bridge void *)(self));
        }
        
        // Create properties
        CMTime presentationTimeStamp = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
        CMTime duration = CMSampleBufferGetDuration(sampleBuffer);
        VTEncodeInfoFlags flags;
        
        // Pass it to the encoder
        OSStatus statusCode = VTCompressionSessionEncodeFrame(encodingSession,
                                                              imageBuffer,
                                                              presentationTimeStamp,
                                                              duration,
                                                              NULL, NULL, &flags);
        // Check for error
        if (statusCode != noErr) {
//            // End the session
//            VTCompressionSessionInvalidate(encodingSession);
//            CFRelease(encodingSession);
//            encodingSession = NULL;
//            
//            if (rtp) {
//                delete rtp;
//                rtp = NULL;
//            }
            
            NSLog(@"H264 encode: VTCompressionSessionEncodeFrame error : %d", (int)statusCode);
            [self.delegate videoEncoder:self error:@"VTCompressionSessionEncodeFrame failed"];
            return;
        }
    });
}

- (void)end
{
    if (encodingSession != NULL) {
        // Mark the completion
        VTCompressionSessionCompleteFrames(encodingSession, kCMTimeInvalid);
        
        // End the session
        VTCompressionSessionInvalidate(encodingSession);
        CFRelease(encodingSession);
        encodingSession = NULL;
    }
    
    if (rtp) {
        delete rtp;
        rtp = NULL;
    }
}

@end
