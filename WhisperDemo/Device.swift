//
//  Device.swift
//  WhisperDemo
//
//  Created by suleyu on 2017/3/9.
//  Copyright © 2017年 Kortide. All rights reserved.
//

import Foundation
import ManagedWhisper

class Device : Equatable {
    var deviceInfo : WhisperFriendInfo
    
    init(_ friend: WhisperFriendInfo) {
        deviceInfo = friend
    }
    
    var deviceId : String {
        get {
            return deviceInfo.userInfo!.userId!
        }
    }
    
    var deviceName : String {
        get {
            if !(deviceInfo.label!.isEmpty) {
                return deviceInfo.label!
            }
            else if !(deviceInfo.userInfo!.name!.isEmpty) {
                return deviceInfo.userInfo!.name!
            }
            else {
                return deviceInfo.userInfo!.userId!
            }
        }
    }
}

func ==(lhs: Device, rhs: Device) -> Bool {
    return lhs.deviceInfo == rhs.deviceInfo
}
