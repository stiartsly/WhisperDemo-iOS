import Foundation
import QRCode
#if USE_VANILLA
import WhisperVanilla
#endif
#if USE_ORCHID
import WhisperOrchid
#endif

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
                let address = whisperInst.getAddress()
                var qrCode = QRCode(address)
                qrCode?.size = CGSize(width: qrCodeWidth, height: qrCodeWidth)
                qrCodeImageView.image = qrCode?.image

                return
            }
        }

        messageLabel.text = "尚未成功连接服务器"
    }
}
