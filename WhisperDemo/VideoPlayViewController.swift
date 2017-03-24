//
//  VideoPlayViewController.swift
//  WhisperDemo
//
//  Created by suleyu on 2017/3/6.
//  Copyright © 2017年 Kortide. All rights reserved.
//

import Foundation
import AVFoundation
import ManagedWhisper

class VideoPlayViewController : UIViewController {
    var device : Device?
    private var _observer : NSObjectProtocol?
    
    private var playLayer = AVSampleBufferDisplayLayer()
    
    deinit {
        if let observer = _observer {
            NotificationCenter.default.removeObserver(observer)
        }
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        self.view.backgroundColor = UIColor.black
        
        if let deviceInfo = device {
            navigationItem.title = device?.deviceName
            
            _observer = NotificationCenter.default.addObserver(forName: DeviceManager.DeviceListChanged, object: nil, queue: OperationQueue.main, using: {
                [unowned self] _ in
                if DeviceManager.sharedInstance.status == .Connected {
                    if DeviceManager.sharedInstance.devices.contains(deviceInfo) {
                        self.navigationItem.title = self.device?.deviceName
                    }
                }
            })
        }
        else {
            navigationItem.title = "本机"
        }
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        playLayer.frame = self.view.bounds
        playLayer.videoGravity = AVLayerVideoGravityResizeAspect
        //playLayer.setAffineTransform(CGAffineTransform(rotationAngle: CGFloat(M_PI_2)))
        self.view.layer.insertSublayer(playLayer, at: 0)
        
        DeviceManager.sharedInstance.startVideoPlay(playLayer, device: self.device)
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        DeviceManager.sharedInstance.stopVideoPlay(self.device)
    }
    
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        super.viewWillTransition(to: size, with: coordinator)
        playLayer.frame = CGRect(origin: CGPoint.zero, size: size)
    }

//
//    func rotateLayer() {
//    let orientation = UIDevice.current.orientation;
//    switch (orientation) {
//    case .portrait:
//    playLayer.connection.videoOrientation = .portrait;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(0.0);
//    break;
//    
//    case UIDeviceOrientationPortraitUpsideDown:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI); // 180 degrees
//    break;
//    
//    case UIDeviceOrientationLandscapeLeft:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI+ M_PI_2); // 270 degrees
//    break;
//    
//    case UIDeviceOrientationLandscapeRight:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI_2); // 90 degrees
//    break;
//    
//    default:
//    {
//    UIInterfaceOrientation statusBarOrientation = [UIApplication sharedApplication].statusBarOrientation;
//    switch (statusBarOrientation) {
//    case UIInterfaceOrientationPortrait:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationPortrait;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(0.0);
//    break;
//    
//    case UIInterfaceOrientationPortraitUpsideDown:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI); // 180 degrees
//    break;
//    
//    case UIInterfaceOrientationLandscapeLeft:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI_2); // 90 degrees
//    break;
//    
//    case UIInterfaceOrientationLandscapeRight:
//    _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
//    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI+ M_PI_2); // 270 degrees
//    break;
//    
//    default:
//    break;
//    }
//    }
//    break;
//    }
//    }
}
