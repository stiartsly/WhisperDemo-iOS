//
//  DeviceViewController.swift
//  WhisperDemo
//
//  Created by suleyu on 2017/1/9.
//  Copyright © 2017年 Kortide. All rights reserved.
//

import Foundation
import MBProgressHUD
import ManagedWhisper

class DeviceViewController: UITableViewController {
    var device : Device?
    var hud : MBProgressHUD?
    
    private var _observer : NSObjectProtocol?
    
    @IBOutlet weak var bulbStatus: UIButton!
    @IBOutlet weak var bulbSwitch: UISwitch!
    @IBOutlet weak var torchSwitch: UISwitch!
    @IBOutlet weak var brightnessSlider: UISlider!
    @IBOutlet weak var audioPlayButton: UIButton!
    @IBOutlet weak var volumeSlider: UISlider!
    
    deinit {
        if let observer = _observer {
            NotificationCenter.default.removeObserver(observer)
        }
        NotificationCenter.default.removeObserver(self)
    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        navigationItem.leftItemsSupplementBackButton = true
        navigationItem.rightBarButtonItem = UIBarButtonItem(image: UIImage(named: "setting"), style: .plain, target: self, action: #selector(configDeviceInfo))
        navigationItem.backBarButtonItem = UIBarButtonItem(title: "返回", style: .plain, target: self, action: nil)
        
        self.tableView.tableFooterView = UIView(frame: CGRect.zero)
        self.tableView.layoutMargins = UIEdgeInsets.zero
        
        if let deviceInfo = self.device {
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
        
        NotificationCenter.default.addObserver(self, selector: #selector(onReceivedStatus), name: DeviceManager.DeviceStatusChanged, object: nil)
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        self.tableView.separatorStyle = splitViewController!.isCollapsed ? .singleLine : .none
        
        if self.device != nil {
            hud = MBProgressHUD(view: self.view)
            self.view.addSubview(hud!)
            hud!.graceTime = 1
            hud!.show(animated: true)
        }
        
        if let status = try! DeviceManager.sharedInstance.getDeviceStatus(device) {
            let bulbStatus = status["bulb"] as! Bool
            self.bulbStatus.isSelected = bulbStatus
            self.bulbSwitch.setOn(bulbStatus, animated: false)
            
            let torchStatus = status["torch"] as! String
            switch torchStatus {
            case "on":
                self.torchSwitch.setOn(true, animated: false)
                self.torchSwitch.isEnabled = true
            case "off":
                self.torchSwitch.setOn(false, animated: false)
                self.torchSwitch.isEnabled = true
            default:
                self.torchSwitch.thumbTintColor = UIColor.lightGray
                self.torchSwitch.setOn(false, animated: false)
                self.torchSwitch.isEnabled = false
            }
            
            self.brightnessSlider.value = status["brightness"] as! Float
            self.audioPlayButton.isSelected = status["audioPlay"] as! Bool
            self.volumeSlider.value = status["volume"] as! Float
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
    }

    override func tableView(_ tableView: UITableView, willDisplay cell: UITableViewCell, forRowAt indexPath: IndexPath) {
        cell.layoutMargins = UIEdgeInsets.zero
        
        if (self.splitViewController!.isCollapsed) {
            cell.backgroundColor = UIColor.white
            cell.backgroundView = nil
        }
        else {
            let cornerRadius : CGFloat = 6.0
            cell.backgroundColor = UIColor.clear
            let layer: CAShapeLayer = CAShapeLayer()
            let pathRef:CGMutablePath = CGMutablePath()
            let bounds: CGRect = cell.bounds.insetBy(dx: 10, dy: 0)
            var addLine: Bool = false
            
            if (indexPath.row == 0 && indexPath.row == tableView.numberOfRows(inSection: indexPath.section)-1) {
                pathRef.__addRoundedRect(transform: nil, rect: bounds, cornerWidth: cornerRadius, cornerHeight: cornerRadius)
            } else if (indexPath.row == 0) {
                pathRef.move(to: CGPoint(x: bounds.minX, y: bounds.maxY))
                pathRef.addArc(tangent1End: CGPoint(x: bounds.minX, y: bounds.minY), tangent2End: CGPoint(x: bounds.midX, y: bounds.minY), radius: cornerRadius)
                pathRef.addArc(tangent1End: CGPoint(x: bounds.maxX, y: bounds.minY), tangent2End: CGPoint(x: bounds.maxX, y: bounds.midY), radius: cornerRadius)
                pathRef.addLine(to: CGPoint(x: bounds.maxX, y: bounds.maxY))
                addLine = true
            } else if (indexPath.row == tableView.numberOfRows(inSection: indexPath.section)-1) {
                pathRef.move(to: CGPoint(x: bounds.minX, y: bounds.minY))
                pathRef.addArc(tangent1End: CGPoint(x: bounds.minX, y: bounds.maxY), tangent2End: CGPoint(x: bounds.midX, y: bounds.maxY), radius: cornerRadius)
                pathRef.addArc(tangent1End: CGPoint(x: bounds.maxX, y: bounds.maxY), tangent2End: CGPoint(x: bounds.maxX, y: bounds.midY), radius: cornerRadius)
                pathRef.addLine(to: CGPoint(x: bounds.maxX, y: bounds.minY))
            } else {
                pathRef.addRect(bounds)
                addLine = true
            }
            
            layer.path = pathRef
            layer.fillColor = UIColor.white.cgColor
            
            if (addLine == true) {
                let lineLayer: CALayer = CALayer()
                let lineHeight: CGFloat = (1.0 / UIScreen.main.scale)
                lineLayer.frame = CGRect(origin: CGPoint(x: bounds.minX+8, y: bounds.size.height-lineHeight), size: CGSize(width: bounds.size.width-8, height: lineHeight))
                lineLayer.backgroundColor = tableView.separatorColor?.cgColor
                layer.addSublayer(lineLayer)
            }
            
            let testView: UIView = UIView(frame: bounds)
            testView.layer.insertSublayer(layer, at: 0)
            testView.backgroundColor = UIColor.clear
            cell.backgroundView = testView
        }
    }
    
    override func viewWillTransition(to size: CGSize, with coordinator: UIViewControllerTransitionCoordinator) {
        super.viewWillTransition(to: size, with: coordinator)
        tableView.reloadData()
    }
    
    override func willTransition(to newCollection: UITraitCollection, with coordinator: UIViewControllerTransitionCoordinator) {
        super.willTransition(to: newCollection, with: coordinator)
        tableView.separatorStyle = newCollection.horizontalSizeClass == .regular ? .none : .singleLine
    }
    
    @IBAction func bulbSwitchChanged(_ sender: UISwitch) {
        let newStatus = sender.isOn
        print("bulbSwitchChanged : \(newStatus)")
        
        do {
            try DeviceManager.sharedInstance.setBulbStatus(newStatus, device: self.device)
            hud?.show(animated: true)
        }
        catch {
            NSLog("open/close bulb failed : \(error)")
            DispatchQueue.main.async {
                sender.setOn(!newStatus, animated: true)
            }
        }
    }
    
    @IBAction func torchSwitchChanged(_ sender: UISwitch) {
        let newStatus = sender.isOn
        print("torchSwitchChanged : \(newStatus)")
        
        do {
            try DeviceManager.sharedInstance.setTorchStatus(newStatus, device: self.device)
            hud?.show(animated: true)
        }
        catch {
            NSLog("open/close torch failed : \(error)")
            DispatchQueue.main.async {
                sender.setOn(!newStatus, animated: true)
            }
        }
    }
    
    @IBAction func brightnessSliderValueChanged(_ sender: UISlider) {
        print("brightnessSliderValueChanged : \(sender.value)")
        do {
            try DeviceManager.sharedInstance.setBrightness(sender.value, device: self.device)
            hud?.show(animated: true)
        }
        catch {
            NSLog("set brightness failed : \(error)")
        }
    }
    
    @IBAction func audioPlayButtonClicked(_ sender: UIButton) {
        do {
            if sender.isSelected {
                try DeviceManager.sharedInstance.stopAudioPlay(self.device)
                sender.isSelected = false
            } else {
                try DeviceManager.sharedInstance.startAudioPlay(self.device)
                sender.isSelected = true
            }
            
            hud?.show(animated: true)
        }
        catch {
            NSLog("set brightness failed : \(error)")
        }
    }
    
    @IBAction func volumeSliderValueChanged(_ sender: UISlider) {
        print("volumeSliderValueChanged : \(sender.value)")
        do {
            try DeviceManager.sharedInstance.setVolume(sender.value, device: self.device)
            hud?.show(animated: true)
        }
        catch {
            NSLog("set volume failed : \(error)")
        }
    }
    
    @objc private func onReceivedStatus(_ notification: Notification) {
        guard device?.deviceId == (notification.object as? String) else {
            return
        }
        
        if let status = notification.userInfo {
            DispatchQueue.main.async {
                if let bulbStatus = status["bulb"] as? Bool {
                    self.bulbStatus.isSelected = bulbStatus
                    self.bulbSwitch.setOn(bulbStatus, animated: false)
                }
                if let torchStatus = status["torch"] as? String {
                    switch torchStatus {
                    case "on":
                        self.torchSwitch.setOn(true, animated: false)
                        self.torchSwitch.isEnabled = true
                    case "off":
                        self.torchSwitch.setOn(false, animated: false)
                        self.torchSwitch.isEnabled = true
                    default:
                        self.torchSwitch.thumbTintColor = UIColor.lightGray
                        self.torchSwitch.setOn(false, animated: false)
                        self.torchSwitch.isEnabled = false
                    }
                }
                
                if let brightness = status["brightness"] as? Float {
                    self.brightnessSlider.value = brightness
                }
                
                if let audioPlay = status["audioPlay"] as? Bool {
                    self.audioPlayButton.isSelected = audioPlay
                }
                
                if let volume = status["volume"] as? Float {
                    self.volumeSlider.value = volume
                }
                
                self.hud?.hide(animated: true)
            }
        }
    }
    
    @objc private func configDeviceInfo() {
        let deviceInfoVC = DeviceInfoViewController()
        deviceInfoVC.device = self.device
        navigationController?.show(deviceInfoVC, sender: nil)
    }
}
