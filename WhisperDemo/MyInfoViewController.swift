//
//  MyInfoViewController.swift
//  WhisperDemo
//
//  Created by suleyu on 2017/1/3.
//  Copyright © 2017年 Kortide. All rights reserved.
//

import Foundation
import ManagedWhisper
import QRCode

class MyInfoController: UIViewController {
    
    @IBOutlet weak var nameLabel: UILabel!
    @IBOutlet weak var messageLabel: UILabel!
    @IBOutlet weak var qrCodeImageView: UIImageView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        navigationItem.title = "我"

        if let whisperInst = DeviceManager.sharedInstance.whisperInst {
            if let myInfo = try? whisperInst.getSelfUserInfo() {
                nameLabel.text = myInfo.name;
                
                let qrCodeWidth = qrCodeImageView.bounds.size.width * UIScreen.main.scale;
                var qrCode = QRCode(myInfo.userId!)
                qrCode?.size = CGSize(width: qrCodeWidth, height: qrCodeWidth)
                qrCodeImageView.image = qrCode?.image

                return
            }
        }

        messageLabel.text = "尚未成功连接服务器"
    }
}
