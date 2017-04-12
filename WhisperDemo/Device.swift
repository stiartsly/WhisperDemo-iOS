//
//  Device.swift
//  WhisperDemo
//
//  Created by suleyu on 2017/3/9.
//  Copyright © 2017年 Kortide. All rights reserved.
//

import Foundation
import AVFoundation
import ManagedWhisper

class Device {
    var deviceInfo : WhisperFriendInfo
    fileprivate var semaphore : DispatchSemaphore?
    
    var deviceId : String {
        get {
            return deviceInfo.userId!
        }
    }
    
    var deviceName : String {
        get {
            if !(deviceInfo.label!.isEmpty) {
                return deviceInfo.label!
            }
            else if !(deviceInfo.name!.isEmpty) {
                return deviceInfo.name!
            }
            else {
                return deviceInfo.userId!
            }
        }
    }
    
// MARK: - Video variables
    
    var session : WhisperSession?
    var stream : WhisperStream?
    var state: WhisperStreamState?
    
    /// remove play local video
    var remotePlaying : Bool = false {
        willSet {
            if !newValue && videoPlayLayer == nil {
                closeSession()
            }
        }
    }
    
    /// local play remove video
    var videoPlayLayer : AVSampleBufferDisplayLayer?
    fileprivate var decoder : VideoDecoder?
    
// MARK: - Methods
    
    init(_ friend: WhisperFriendInfo) {
        deviceInfo = friend
    }
    
    deinit {
        closeSession()
    }
    
    func closeSession() {
        if let session = self.session {
            decoder?.end()
            
            if let stream = self.stream {
                try? session.removeStream(stream: stream)
                self.stream = nil
            }
            session.close()
            self.session = nil
            
            self.state = nil
        }
    }
}

// MARK: - WhisperStreamDelegate
extension Device : WhisperStreamDelegate
{
    func streamStateDidChange(_ stream: WhisperStream, _ newState: WhisperStreamState) {
        guard stream == self.stream else {
            return
        }
        
        print("streamStateDidChange : \(newState)")
        
        state = newState
        if newState == .Connected {
            if videoPlayLayer != nil {
                let messageDic = ["type":"modify", "videoPlay":true] as [String : Any]
                try! DeviceManager.sharedInstance.sendMessage(messageDic, toDevice: self)
            }
        }
        else if newState == .Error {
            closeSession()
        }
        
        self.semaphore?.signal()
    }
    
    func didReceiveStreamData(_ stream: WhisperStream, _ component: Int, _ data: Data) {
        if decoder == nil {
            decoder = VideoDecoder()
            decoder?.delegate = self
        }
        decoder?.decode(data)
    }
    
    func didReceiveSessionInviteRequest(whisper: Whisper, sdp: String) -> Bool {
        objc_sync_enter(self)
        defer {
            objc_sync_exit(self)
        }
        
        do {
            if stream == nil {
                if session == nil {
                    session = try WhisperSessionManager.getInstance()!.newSession(to: self.deviceId+"@"+self.deviceId)
                }
                
                semaphore = DispatchSemaphore(value: 0)
                defer {
                    semaphore = nil
                }
                
                stream = try session!.addStream(type: WhisperStreamType.Video, options: [], components: 1, delegate: self)
                semaphore?.wait()
                guard state == .CandidateGathered else {
                    return false
                }
                
                try session!.replyInviteRequest(with: 0, reason: nil)
                semaphore?.wait()
                guard state == .IceReady else {
                    return false
                }
                
                try session!.start(remoteSdp: sdp)
                return true
            }
            else if state == .IceReady {
                try session!.replyInviteRequest(with: 0, reason: nil)
                try session!.start(remoteSdp: sdp)
                return true
            }
            else {
                return false
            }
        }
        catch {
            NSLog("didReceiveSessionInviteRequest, start session error: \(error)")
            closeSession()
            return false
        }
    }
}

// MARK: - Video methods
extension Device : VideoDecoderDelegate
{
    /// start play remote video
    func startVideoPlay(_ layer : AVSampleBufferDisplayLayer) -> Bool {
        objc_sync_enter(self)
        defer {
            objc_sync_exit(self)
        }
        
        do {
            if stream == nil {
                if session == nil {
                    session = try WhisperSessionManager.getInstance()!.newSession(to: self.deviceId+"@"+self.deviceId)
                }
                
                semaphore = DispatchSemaphore(value: 0)
                defer {
                    semaphore = nil
                }
                
                stream = try session!.addStream(type: WhisperStreamType.Video, options: [], components: 1, delegate: self)
                semaphore?.wait()
                guard state == .CandidateGathered else {
                    return false
                }
                
                try session!.sendInviteRequest(handler: didReceiveSessionInviteResponse)
                semaphore?.wait()
                guard state == .IceReady else {
                    return false
                }
            }
            else if state == .IceReady {
                try session!.sendInviteRequest(handler: didReceiveSessionInviteResponse)
            }
            else if state == .Connected {
                let messageDic = ["type":"modify", "videoPlay":true] as [String : Any]
                try! DeviceManager.sharedInstance.sendMessage(messageDic, toDevice: self)
            }
            
            videoPlayLayer = layer
            return true
        }
        catch {
            closeSession()
            return false
        }
    }
    
    private func didReceiveSessionInviteResponse(session: WhisperSession, status: Int, reason: String?, sdp: String?) {
        guard state == .IceReady else {
            return
        }
        
        if status == 0 {
            NSLog("didReceiveSessionInviteResponse success")
            try! session.start(remoteSdp: sdp!)
            NSLog("Start session in success")
        } else {
            NSLog("didReceiveSessionInviteResponse failed with reason: \(reason!)")
            videoPlayLayer = nil
        }
    }
    
    /// stop play remote video
    func stopVideoPlay() {
        if remotePlaying {
            decoder?.end()
        }
        else {
            closeSession()
        }
        
        decoder = nil
        videoPlayLayer = nil
        
        let messageDic = ["type":"modify", "videoPlay":false] as [String : Any]
        try? DeviceManager.sharedInstance.sendMessage(messageDic, toDevice: self)
    }
    
// MARK: VideoDecoderDelegate
    
    func videoDecoder(_ encoder: VideoDecoder!, gotSampleBuffer sampleBuffer: CMSampleBuffer!) {
        if let playLayer = videoPlayLayer {
            //if playLayer.isReadyForMoreMediaData {
                DispatchQueue.main.sync {
                    playLayer.enqueue(sampleBuffer)
                    if playLayer.status == .failed {
                        playLayer.flush()
                    }
                    else {
                        playLayer.setNeedsDisplay()
                    }
                }
            //}
        }
    }
    
    func videoDecoder(_ encoder: VideoDecoder!, error: String!) {
        
    }
}

// MARK: - Hashable
extension Device : Hashable
{
    var hashValue : Int {
        get {
            return self.deviceId.hashValue
        }
    }
}

// MARK: - Equatable
extension Device : Equatable {}

func ==(lhs: Device, rhs: Device) -> Bool {
    return lhs.deviceInfo == rhs.deviceInfo
}
