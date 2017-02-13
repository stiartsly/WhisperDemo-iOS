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
        
        if let whisper = Whisper.getInstance() {
            let myInfo = try! whisper.getSelfInfo()
            nameLabel.text = myInfo.name;
            
            let qrCodeWidth = qrCodeImageView.bounds.size.width * UIScreen.main.scale;
            var qrCode = QRCode(myInfo.userId!)
            qrCode?.size = CGSize(width: qrCodeWidth, height: qrCodeWidth)
            qrCodeImageView.image = qrCode?.image
        }
        else {
            messageLabel.text = "连接服务器失败"
        }
    }
}
