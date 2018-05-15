import Foundation
import AVFoundation
#if USE_VANILLA
import WhisperVanilla
#endif
#if USE_ORCHID
import WhisperOrchid
#endif

class Device {
    var deviceInfo : WhisperFriendInfo
    fileprivate var semaphore : DispatchSemaphore?

    var didReceiveSessionResponse: Bool = false
    var remoteSdp: String?
    
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

    var status : [String: Any]?
    
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
    var videoPlayView : UIImageView?
    var videoPlayLayer : AVSampleBufferDisplayLayer?
    fileprivate var decoder: VideoDecoder?
    
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
                self.stream = nil
                try? session.removeStream(stream: stream)
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
            try? _ = stream.getTransportInfo()

            if videoPlayLayer != nil {
                let messageDic = ["type":"modify", "camera":true] as [String : Any]
                try! DeviceManager.sharedInstance.sendMessage(messageDic, toDevice: self)
                remotePlaying = true;
            }
        }
        else if newState == .Closed || newState == .Deactivated || newState == .Error {
            closeSession()
        }
        
        self.semaphore?.signal()
    }
    
    func didReceiveStreamData(_ stream: WhisperStream, _ data: Data) {
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

#if USE_VANILLA
                let plistPath = Bundle.main.path(forResource: "Config", ofType: "plist")
                let info = NSDictionary(contentsOfFile: plistPath!) as! [String: String]

                let options = IceTransportOptions()
                options.threadModel = TransportOptions.SharedThreadModel
                options.stunHost = info["StunServer"]!
                options.turnHost = info["TurnServer"]!
                options.turnUsername = info["TurnUserName"]!
                options.turnPassword = info["TurnPassword"]!

                if session == nil {
                    session = try WhisperSessionManager.getInstance()!.newSession(to: self.deviceId+"@"+self.deviceId, options);
                }
#elseif USE_ORCHID
                if session == nil {
                    session = try WhisperSessionManager.getInstance()!.newSession(to: self.deviceId);
                }
#else
//MARK: unknown variant.
#endif
                
                semaphore = DispatchSemaphore(value: 0)
                defer {
                    semaphore = nil
                }
                
                stream = try session!.addStream(type: .Application, options: [], delegate: self)
                semaphore?.wait()
                guard state == .Initialized else {
                    return false
                }
                
                try session!.replyInviteRequest(with: 0, reason: nil)
                semaphore?.wait()
                guard state == .TransportReady else {
                    return false
                }
                
                try session!.start(remoteSdp: sdp)
                return true
            }
            else if state == .TransportReady {
                try session!.replyInviteRequest(with: 0, reason: nil)
                try session!.start(remoteSdp: sdp)
                return true
            }
            else {
                return false
            }
        }
        catch {
            NSLog("didReceiveSessionInviteRequest, start session error: \(error.localizedDescription)")
            closeSession()
            return false
        }
    }
}

// MARK: - Video methods
extension Device : VideoDecoderDelegate
{
    /// start play remote video
    func startVideoPlay(_ view : UIImageView, _ layer : AVSampleBufferDisplayLayer) -> Bool {
        objc_sync_enter(self)
        defer {
            objc_sync_exit(self)
        }
        
        do {
            if stream == nil {
                if session == nil {
#if USE_VANILLA
                    let plistPath = Bundle.main.path(forResource: "Config", ofType: "plist")
                    let info = NSDictionary(contentsOfFile: plistPath!) as! [String: String]

                    let options = IceTransportOptions()
                    options.threadModel = TransportOptions.SharedThreadModel
                    options.stunHost = info["StunServer"]!
                    options.turnHost = info["TurnServer"]!
                    options.turnUsername = info["TurnUserName"]!
                    options.turnPassword = info["TurnPassword"]!

                    session = try WhisperSessionManager.getInstance()!.newSession(to: self.deviceId+"@"+self.deviceId, options)
#elseif USE_ORCHID
                    session = try WhisperSessionManager.getInstance()!.newSession(to: self.deviceId)
#else
//MARK: unknown variant.
#endif
                }
                
                semaphore = DispatchSemaphore(value: 0)
                defer {
                    semaphore = nil
                }
                
                stream = try session!.addStream(type: .Application, options: [], delegate: self)
                semaphore?.wait()
                guard state == .Initialized else {
                    return false
                }
                
                try session!.sendInviteRequest(handler: didReceiveSessionInviteResponse)
                semaphore?.wait()
                guard state == .TransportReady else {
                    return false
                }

                while !didReceiveSessionResponse {
                    Thread.sleep(forTimeInterval: 0.1)
                }

                try session!.start(remoteSdp: remoteSdp!)
                NSLog("start session success")
            }
            else if state == .TransportReady {
                try session!.sendInviteRequest(handler: didReceiveSessionInviteResponse)
            }
            else if state == .Connected {
                let messageDic = ["type":"modify", "camera":true] as [String : Any]
                try! DeviceManager.sharedInstance.sendMessage(messageDic, toDevice: self)
            }

            videoPlayView = view
            videoPlayLayer = layer
            return true
        }
        catch {
            closeSession()
            return false
        }
    }
    
    private func didReceiveSessionInviteResponse(session: WhisperSession, status: Int, reason: String?, sdp: String?) {
        guard state == .TransportReady else {
            return
        }
        
        if status == 0 {
            NSLog("didReceiveSessionInviteResponse success")
            didReceiveSessionResponse = true;
            remoteSdp = sdp;
        } else {
            NSLog("didReceiveSessionInviteResponse failed with reason: \(reason!)")
            videoPlayView = nil
            videoPlayLayer = nil
        }
    }
    
    /// stop play remote video
    func stopVideoPlay() {
        videoPlayView = nil
        videoPlayLayer = nil

        if remotePlaying {
            decoder?.end()
        }
        else {
            closeSession()
        }
        
        decoder = nil
        
        let messageDic = ["type":"modify", "camera":false] as [String : Any]
        try? DeviceManager.sharedInstance.sendMessage(messageDic, toDevice: self)
    }
    
// MARK: VideoDecoderDelegate
    
    func videoDecoder(_ decoder: VideoDecoder!, gotSampleBuffer sampleBuffer: CMSampleBuffer!) {
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

    func videoDecoder(_ decoder: VideoDecoder!, gotVideoImage image: UIImage!) {
        DispatchQueue.main.sync {
            videoPlayView?.image = image
        }
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
extension Device : Equatable {
    static func ==(lhs: Device, rhs: Device) -> Bool {
        return lhs.deviceInfo == rhs.deviceInfo
    }
}
