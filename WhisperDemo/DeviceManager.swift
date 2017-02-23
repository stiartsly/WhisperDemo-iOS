//
//  DeviceManager.swift
//  WhisperDemo
//
//  Created by suleyu on 2017/1/12.
//  Copyright © 2017年 Kortide. All rights reserved.
//

import Foundation
import AVFoundation
import MediaPlayer
import ManagedWhisper

class DeviceManager : NSObject, WhisperHandler {
    static let sharedInstance = DeviceManager()
    static let SelfInfoChanged = NSNotification.Name("kNotificationSelfInfoChanged")
    static let DeviceListChanged = NSNotification.Name("kNotificationDeviceListChanged")
    static let DeviceStatusChanged = NSNotification.Name("kNotificationDeviceStatusChanged")
    
    var status = WhisperConnectionStatus.Disconnected;
    var whisperInst: Whisper!
    var devices = [WhisperFriendInfo]()
    
    var bulbStatus = false
    var captureDevice: AVCaptureDevice?
    var audioPlayer : AVAudioPlayer?
    var audioVolume : Float = 1.0
    
    override init() {
        Whisper.setLogLevel(level: 4)
    }
    
    func start() {
        if whisperInst == nil {
            do {
                let whisperDirectory: String = NSSearchPathForDirectoriesInDomains(.libraryDirectory, .userDomainMask, true)[0] + "/whisper"
                if !FileManager.default.fileExists(atPath: whisperDirectory) {
                    var url = URL(fileURLWithPath: whisperDirectory)
                    try FileManager.default.createDirectory(at: url, withIntermediateDirectories: true, attributes: nil)
                    
                    var resourceValues = URLResourceValues()
                    resourceValues.isExcludedFromBackup = true
                    try url.setResourceValues(resourceValues)
                }
                
                let options = WhisperOptions()
                options.appId = "7sRQjDsniyuHdZ9zsQU9DZbMLtQGLBWZ78yHWgjPpTKm"
                options.appKey = "6tzPPAgSACJdScX79wuzMNPQTWkRLZ4qEdhLcZU6q4B9"
//                options.apiServerUrl = "http://192.168.3.182:8080/web/api"
//                options.mqttServerUri = "tcp://192.168.3.182:1883"
//                options.apiServerUrl = "https://192.168.3.182:8080:8443/web/api"
//                options.mqttServerUri = "ssl://192.168.3.182:8883"
                options.apiServerUrl = "https://whisper.freeddns.org:8443/web/api"
                options.mqttServerUri = "ssl://whisper.freeddns.org:8883"
                options.trustStore = Bundle.main.path(forResource: "whisper", ofType: "pem")
                options.persistentLocation = whisperDirectory
                options.deviceId = UIDevice.current.identifierForVendor!.uuidString
                options.connectTimeout = 5
                
//                try? FileManager.default.removeItem(atPath: whisperDirectory + "/.whisper")
                try whisperInst = Whisper.getInstance(with: options, handler: self, context: nil)
                print("Whisper instance created")
                
                try whisperInst.start(iterateInterval: 1000)
                print("Whisper started, waiting for ready")
                
                NotificationCenter.default.addObserver(forName: .UIApplicationWillResignActive, object: nil, queue: OperationQueue.main, using: didEnterBackground)
                NotificationCenter.default.addObserver(forName: .UIScreenBrightnessDidChange, object: nil, queue: OperationQueue.main, using: brightnessDidChanged)
            }
            catch {
                NSLog("Start whisper instance error : \(error)")
            }
        }
    }
    
    func getDeviceStatus(_ device: WhisperFriendInfo? = nil) throws -> [String: Any]? {
        if let deviceInfo = device {
            let messageDic = ["type":"query"]
            try sendMessage(messageDic, toDevice: deviceInfo)
            return nil
        }
        else {
            var selfStstus = [String : Any]()
            selfStstus["type"] = "status"
            selfStstus["bulb"] = bulbStatus
            
            if captureDevice == nil {
                captureDevice = AVCaptureDevice.defaultDevice(withMediaType: AVMediaTypeVideo)
            }
            if captureDevice!.hasTorch && captureDevice!.isTorchAvailable {
                selfStstus["torch"] = captureDevice!.torchMode == .on ? "on" : "off"
            }
            else {
                selfStstus["torch"] = "none"
            }
            
            selfStstus["brightness"] = Float(UIScreen.main.brightness)
            
            if let player = audioPlayer {
                selfStstus["audioPlay"] = player.isPlaying
                selfStstus["volume"] = player.volume
            }
            else {
                selfStstus["audioPlay"] = false
                selfStstus["volume"] = audioVolume
            }
            
            return selfStstus
        }
    }
    
    
    func setBulbStatus(_ on: Bool, device: WhisperFriendInfo? = nil) throws {
        if let deviceInfo = device {
            let messageDic = ["type":"modify", "bulb":on] as [String : Any]
            try sendMessage(messageDic, toDevice: deviceInfo)
        }
        else {
            bulbStatus = on
            
            let messageDic = ["type":"status", "bulb":on] as [String : Any]
            NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
            if (self.status == .Connected) {
                for dev in devices {
                    try? sendMessage(messageDic, toDevice: dev)
                }
            }
        }
    }
    
    func setTorchStatus(_ on: Bool, device: WhisperFriendInfo? = nil) throws {
        if let deviceInfo = device {
            let messageDic = ["type":"modify", "torch":on ? "on" : "off"] as [String : Any]
            try sendMessage(messageDic, toDevice: deviceInfo)
        }
        else {
            if captureDevice == nil {
                captureDevice = AVCaptureDevice.defaultDevice(withMediaType: AVMediaTypeVideo)
            }
            
            try captureDevice!.lockForConfiguration()
            captureDevice!.torchMode = on ? .on : .off;
            captureDevice!.unlockForConfiguration()
            
            let messageDic = ["type":"status", "torch":on ? "on" : "off"] as [String : Any]
            NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
            if (self.status == .Connected) {
                for dev in devices {
                    try? sendMessage(messageDic, toDevice: dev)
                }
            }
        }
    }
    
    private func didEnterBackground(_ notification: Notification) {
        if let tmp = captureDevice {
            if tmp.torchMode == .on {
                try! setTorchStatus(false)
            }
        }
    }
    
    func setBrightness(_ brightness: Float, device: WhisperFriendInfo? = nil) throws {
        if let deviceInfo = device {
            let messageDic = ["type":"modify", "brightness":brightness] as [String : Any]
            try sendMessage(messageDic, toDevice: deviceInfo)
        }
        else {
            UIScreen.main.brightness = CGFloat(brightness)
            
            let messageDic = ["type":"status", "brightness":brightness] as [String : Any]
            NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
            if (self.status == .Connected) {
                for dev in devices {
                    try? sendMessage(messageDic, toDevice: dev)
                }
            }
        }
    }
    
    private func brightnessDidChanged(_ notification: Notification) {
        let brightness = Float((notification.object as! UIScreen).brightness)
        print("UIScreenBrightnessDidChange : \(brightness)")
        
        let messageDic = ["type":"status", "brightness":brightness] as [String : Any]
        NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
        
        if (self.status == .Connected) {
            for dev in devices {
                try? sendMessage(messageDic, toDevice: dev)
            }
        }
    }
    
    func startPlayAudio(_ device: WhisperFriendInfo? = nil) throws {
        if let deviceInfo = device {
            let messageDic = ["type":"modify", "audioPlay":true] as [String : Any]
            try sendMessage(messageDic, toDevice: deviceInfo)
        }
        else {
            if audioPlayer == nil {
                do {
                    try AVAudioSession.sharedInstance().setCategory(AVAudioSessionCategoryPlayback)
                    let _ = try AVAudioSession.sharedInstance().setActive(true)
                } catch let error as NSError {
                    print("an error occurred when audio session category.\n \(error)")
                }
                
                let path = Bundle.main.url(forResource: "audio", withExtension: "m4a")
                try! audioPlayer = AVAudioPlayer(contentsOf: path!)
                audioPlayer!.numberOfLoops = -1
                audioPlayer!.volume = audioVolume
            }
            
            audioPlayer!.play()
            NotificationCenter.default.addObserver(self, selector: #selector(audioSessionInterrupted), name: .AVAudioSessionInterruption, object: AVAudioSession.sharedInstance())
            
            let messageDic = ["type":"status", "audioPlay":true] as [String : Any]
            NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
            if (self.status == .Connected) {
                for dev in devices {
                    try? sendMessage(messageDic, toDevice: dev)
                }
            }
        }
    }
    
    func stopPlayAudio(_ device: WhisperFriendInfo? = nil) throws {
        if let deviceInfo = device {
            let messageDic = ["type":"modify", "audioPlay":false] as [String : Any]
            try sendMessage(messageDic, toDevice: deviceInfo)
        }
        else {
            if let player = audioPlayer {
                player.stop()
            }
            
            NotificationCenter.default.removeObserver(self, name: .AVAudioSessionInterruption, object: AVAudioSession.sharedInstance())
            
            let messageDic = ["type":"status", "audioPlay":false] as [String : Any]
            NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
            if (self.status == .Connected) {
                for dev in devices {
                    try? sendMessage(messageDic, toDevice: dev)
                }
            }
        }
    }
    
    @objc private func audioSessionInterrupted(_ notification:Notification)
    {
        print("audioSessionInterrupted: \(notification)")
        NotificationCenter.default.removeObserver(self, name: .AVAudioSessionInterruption, object: AVAudioSession.sharedInstance())
        
        let messageDic = ["type":"status", "audioPlay":false] as [String : Any]
        NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
        if (self.status == .Connected) {
            for dev in devices {
                try? sendMessage(messageDic, toDevice: dev)
            }
        }
    }
    
    func setVolume(_ volume: Float, device: WhisperFriendInfo? = nil) throws {
        if let deviceInfo = device {
            let messageDic = ["type":"modify", "volume":volume] as [String : Any]
            try sendMessage(messageDic, toDevice: deviceInfo)
        }
        else {
            audioVolume = volume
            audioPlayer?.volume = volume
            
            let messageDic = ["type":"status", "volume":volume] as [String : Any]
            NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: nil, userInfo: messageDic)
            
            if (self.status == .Connected) {
                for dev in devices {
                    try? sendMessage(messageDic, toDevice: dev)
                }
            }
        }
    }
    
    private func sendMessage(_ message: [String: Any], toDevice device: WhisperFriendInfo) throws {
        if device.presence == "online" {
            try sendMessage(message, toDeviceId: device.userInfo!.userId!)
        }
        else {
            throw WhisperError.InternalError(errno:1)
        }
    }
    
    private func sendMessage(_ message: [String: Any], toDeviceId deviceId: String) throws {
        let jsonData = try JSONSerialization.data(withJSONObject: message, options: .prettyPrinted)
        let jsonString = NSString(data: jsonData, encoding: String.Encoding.utf8.rawValue)! as String
        try whisperInst.sendFriendMessage(to: deviceId, with: jsonString)
    }

// MARK: - WhisperHandler

//    func onIdle(w whisper: ManagedWhisper.Whisper, _ context: AnyObject?) {
//        print("onIdle")
//    }
    
    public func onConnection(w whisper: ManagedWhisper.Whisper, with status: ManagedWhisper.WhisperConnectionStatus, _ context: AnyObject?) {
        print("onConnection status : \(status)")
        self.status = status
        if status == .Disconnected {
            self.devices.removeAll()
        }
        
        NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
    }
    
    public func onReady(w whisper: ManagedWhisper.Whisper, _ context: AnyObject?) {
        print("onReady")
        let myInfo = try! whisper.getSelfInfo()
        if myInfo.name?.isEmpty ?? true {
            myInfo.name = UIDevice.current.name
            try? whisper.setSelfInfo(with: myInfo)
        }
        
        //self.devices = try! whisper.getFriends()
        //NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
    }
    
    public func onSelfInfoChanged(w whisper: ManagedWhisper.Whisper, with info: ManagedWhisper.WhisperUserInfo, _ context: AnyObject?) {
        print("onSelfInfoChanged : \(info)")
        NotificationCenter.default.post(name: DeviceManager.SelfInfoChanged, object: nil)
    }
    
    public func onFriendIterated(w whisper: ManagedWhisper.Whisper, with info: ManagedWhisper.WhisperFriendInfo?, _ context: AnyObject?) -> Bool {
        print("onFriendIterated : \(info)")
        if let deviceInfo = info {
            self.devices.append(deviceInfo)
        }
        else {
            NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
        }
        return true;
    }
    
    public func onFriendInfoChanged(w whisper: ManagedWhisper.Whisper, at friendId: String, with info: ManagedWhisper.WhisperFriendInfo, _ context: AnyObject?) {
        print("onFriendInfoChanged")
        for index in 0..<self.devices.count {
            let device = self.devices[index]
            if device.userInfo?.userId == friendId {
                self.devices.replaceSubrange(index...index, with: [info])
                
                NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
                break
            }
        }
    }
    
    public func onFriendPresence(w whisper: ManagedWhisper.Whisper, at friendId: String, with presence: String, _ context: AnyObject?) {
        print("onFriendPresence")
        for device in self.devices {
            if device.userInfo?.userId == friendId {
                device.presence = presence
                
                NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
                break
            }
        }
    }
    
    public func onFriendRequest(w whisper: ManagedWhisper.Whisper, from userId: String, with info: ManagedWhisper.WhisperUserInfo, hello: String, _ context: AnyObject?) -> Bool {
        print("onFriendRequest, userId : \(userId), name : \(info.name), hello : \(hello)")
        var result = false
        do {
            try whisper.replyFriendRequest(from: userId, on: 0, with: nil, entrusted: 0, expire: nil)
            result = true
        } catch {
            NSLog("replyFriendRequest error : \(error)")
        }
        return result;
    }
    
    public func onFriendResponse(w whisper: ManagedWhisper.Whisper, from userId: String, on status: Int, with reason: String?, entrusted: Int, expire: String?, _ context: AnyObject?) -> Bool {
        print("onFriendResponse, userId : \(userId)")
        return true;
    }
    
    public func onFriendAdded(w whisper: ManagedWhisper.Whisper, with info: ManagedWhisper.WhisperFriendInfo, _ context: AnyObject?) {
        print("onFriendAdded")
        self.devices.append(info)
        NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
    }
    
    public func onFriendRemoved(w whisper: ManagedWhisper.Whisper, at friendId: String, _ context: AnyObject?) {
        print("onFriendRemoved")
        for index in 0..<self.devices.count {
            let device = self.devices[index]
            if device.userInfo?.userId == friendId {
                self.devices.remove(at: index)
                
                NotificationCenter.default.post(name: DeviceManager.DeviceListChanged, object: nil)
                break
            }
        }
    }
    
    public func onFriendMessage(w whisper: ManagedWhisper.Whisper, from: String, with message: String, _ context: AnyObject?) -> Bool {
        print("onFriendMessage: \(message)")
        do {
            let data = message.data(using: .utf8)
            let decoded = try JSONSerialization.jsonObject(with: data!, options: [])
            let dict = decoded as! [String: Any]
            let msgType = dict["type"] as! String
            switch msgType {
            case "query":
                let message = try getDeviceStatus()
                try sendMessage(message!, toDeviceId: from)
                
            case "status":
                let userId = from.components(separatedBy: "@")[0]
                NotificationCenter.default.post(name: DeviceManager.DeviceStatusChanged, object: userId, userInfo: dict)
                
            case "modify":
                if let bulb = dict["bulb"] as? Bool {
                    try! setBulbStatus(bulb)
                }
                if let torchStatus = dict["torch"] as? String {
                    try! setTorchStatus(torchStatus == "on")
                }
                if let brightness = dict["brightness"] as? Float {
                    try! setBrightness(brightness)
                }
                if let audioPlay = dict["audioPlay"] as? Bool {
                    if audioPlay {
                        try! startPlayAudio()
                    }
                    else {
                        try! stopPlayAudio()
                    }
                }
                if let volume = dict["volume"] as? Float {
                    try! setVolume(volume)
                }
            default:
                print("unsupported message")
            }
        } catch {
            print(error.localizedDescription)
        }
        return true;
    }
    
    public func onFriendInvite(w whisper: ManagedWhisper.Whisper, from: String, with data: String, _ context: AnyObject?) -> Bool {
        print("onFriendInvite")
        return false;
    }
}
