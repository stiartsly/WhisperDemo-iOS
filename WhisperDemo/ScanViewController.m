
//
//  ScanViewController.m
//  WhisperDemo
//
//  Created by suleyu on 2017/1/3.
//  Copyright © 2017年 Kortide. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "ScanViewController.h"
#import "MBProgressHUD.h"
#import "WhisperDemo-Swift.h"

@interface ScanViewController () <AVCaptureMetadataOutputObjectsDelegate>
{
    BOOL upOrdown;
    NSTimer * timer;
}
@property (assign,nonatomic) BOOL isFlashLightOn;
@property (strong,nonatomic) AVCaptureDevice *device;
@property (strong,nonatomic) AVCaptureDeviceInput *input;
@property (strong,nonatomic) AVCaptureMetadataOutput *output;
@property (strong,nonatomic) AVCaptureSession *session;
@property (strong,nonatomic) AVCaptureVideoPreviewLayer *preview;

@property (weak, nonatomic) IBOutlet UIView *referenceView;
@property (weak, nonatomic) IBOutlet UIImageView *lineView;

@end

@implementation ScanViewController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.navigationItem.title = NSLocalizedString(@"添加设备", nil);
}

-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self setupCamera];
    [self startScan];
}

-(void)viewWillDisappear:(BOOL)animated
{
    upOrdown = NO;
    self.isFlashLightOn = NO;
    [_session stopRunning];
    [timer invalidate];
    timer = nil;
    [super viewWillDisappear:animated];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewWillTransitionToSize:(CGSize)size withTransitionCoordinator:(id <UIViewControllerTransitionCoordinator>)coordinator
{
    [super viewWillTransitionToSize:size withTransitionCoordinator:coordinator];
    [self rotateLayer];
    _preview.frame = CGRectMake(0, 0, size.width, size.height);
}

- (void)setupCamera
{
    if ([AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo] == AVAuthorizationStatusDenied) {
        UIAlertController *alertController = [UIAlertController alertControllerWithTitle:nil message:NSLocalizedString(@"请在iPhone的“设置-隐私-相机”中允许使用相机", nil) preferredStyle:UIAlertControllerStyleAlert];
        [alertController addAction:[UIAlertAction actionWithTitle:NSLocalizedString(@"确定", nil) style:UIAlertActionStyleDefault handler:nil]];
        [self presentViewController:alertController animated:YES completion:nil];
        return;
    }

    _device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    _input = [AVCaptureDeviceInput deviceInputWithDevice:self.device error:nil];

    _output = [[AVCaptureMetadataOutput alloc] init];
    [_output setMetadataObjectsDelegate:self queue:dispatch_get_main_queue()];
    //[_output setRectOfInterest: [self makeScanReaderInterestRect]];

    _session = [[AVCaptureSession alloc]init];
    [_session setSessionPreset:AVCaptureSessionPresetHigh];
    if ([_session canAddInput:self.input])
    {
        [_session addInput:self.input];
    }
    
    if ([_session canAddOutput:self.output])
    {
        [_session addOutput:self.output];
        
        if ([_output.availableMetadataObjectTypes containsObject:AVMetadataObjectTypeQRCode]) {
            _output.metadataObjectTypes = @[AVMetadataObjectTypeQRCode];
        }
    }

    _preview =[AVCaptureVideoPreviewLayer layerWithSession:self.session];
    _preview.videoGravity = AVLayerVideoGravityResizeAspectFill;
    [self rotateLayer];
    _preview.frame = self.view.bounds;
    [self.view.layer insertSublayer:self.preview atIndex:0];
}

-(void)rotateLayer
{
    UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
    switch (orientation) {
        case UIDeviceOrientationPortrait:
            _preview.connection.videoOrientation = AVCaptureVideoOrientationPortrait;
            //_preview.affineTransform = CGAffineTransformMakeRotation(0.0);
            break;
            
        case UIDeviceOrientationPortraitUpsideDown:
            _preview.connection.videoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
            //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI); // 180 degrees
            break;
            
        case UIDeviceOrientationLandscapeLeft:
            _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
            //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI+ M_PI_2); // 270 degrees
            break;
            
        case UIDeviceOrientationLandscapeRight:
            _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
            //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI_2); // 90 degrees
            break;
            
        default:
        {
            UIInterfaceOrientation statusBarOrientation = [UIApplication sharedApplication].statusBarOrientation;
            switch (statusBarOrientation) {
                case UIInterfaceOrientationPortrait:
                    _preview.connection.videoOrientation = AVCaptureVideoOrientationPortrait;
                    //_preview.affineTransform = CGAffineTransformMakeRotation(0.0);
                    break;
                    
                case UIInterfaceOrientationPortraitUpsideDown:
                    _preview.connection.videoOrientation = AVCaptureVideoOrientationPortraitUpsideDown;
                    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI); // 180 degrees
                    break;
                    
                case UIInterfaceOrientationLandscapeLeft:
                    _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeLeft;
                    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI_2); // 90 degrees
                    break;
                    
                case UIInterfaceOrientationLandscapeRight:
                    _preview.connection.videoOrientation = AVCaptureVideoOrientationLandscapeRight;
                    //_preview.affineTransform = CGAffineTransformMakeRotation(M_PI+ M_PI_2); // 270 degrees
                    break;
                    
                default:
                    break;
            }
        }
            break;
    }
}

-(CGRect)makeScanReaderInterestRect
{
    int screenWidth = self.view.bounds.size.width;
    int screenHeight = self.view.bounds.size.height;
    CGRect rect = self.referenceView.frame;

    CGFloat x = rect.origin.y/screenHeight;
    CGFloat y = rect.origin.x/screenWidth;
    CGFloat regionWidth = rect.size.height/screenHeight;
    CGFloat regionHight = rect.size.width/screenWidth;

    return CGRectMake(x, y, regionWidth, regionHight);
}

#pragma mark AVCaptureMetadataOutputObjectsDelegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects:(NSArray *)metadataObjects fromConnection:(AVCaptureConnection *)connection
{
    if ([metadataObjects count] >0)
    {
        [_session stopRunning];
        [timer invalidate];
        timer = nil;
        
        AVMetadataMachineReadableCodeObject *metadataObject = [metadataObjects objectAtIndex:0];
        [self scanComplete :metadataObject.stringValue];
    }
}

-(void)scanLineAnimation
{
    CGRect rect = self.lineView.frame;
    if (upOrdown == NO) {
        rect.origin.y += 2;
        if (rect.origin.y >= self.referenceView.bounds.size.height - rect.size.height) {
            upOrdown = YES;
        }
    }
    else {
        rect.origin.y -= 2;
        if (rect.origin.y < rect.size.height) {
            upOrdown = NO;
        }
    }
    self.lineView.frame = rect;
}

-(void)startScan
{
    [_session startRunning];
    timer = [NSTimer scheduledTimerWithTimeInterval:.02
                                             target:self
                                           selector:@selector(scanLineAnimation)
                                           userInfo:nil
                                            repeats:YES];
}

-(void)scanComplete:(NSString*)deviceID
{
    MBProgressHUD *hud = [MBProgressHUD HUDForView:self.view];
    if (hud) {
        [NSObject cancelPreviousPerformRequestsWithTarget:self];
        hud.mode = MBProgressHUDModeIndeterminate;
        hud.label.text = NSLocalizedString(@"正在处理",nil);
    }
    else {
        hud = [[MBProgressHUD alloc] initWithView:self.view];
        hud.removeFromSuperViewOnHide = YES;
        hud.label.text = NSLocalizedString(@"正在处理",nil);
        [self.view addSubview:hud];
        [hud showAnimated:YES];
    }
    
    __weak __typeof(self) weakSelf = self;
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0), ^{
        NSError *error = nil;
        BOOL result = [[[DeviceManager sharedInstance] whisperInst] sendFriendRequestTo:deviceID withGreeting:@"password" error:&error];
        dispatch_async(dispatch_get_main_queue(), ^{
            __strong __typeof(weakSelf) strongSelf = weakSelf;
            if (strongSelf == nil) {
                return;
            }
            
            if (result) {
                hud.customView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"check_ok"]];
                hud.label.text = NSLocalizedString(@"授权申请已发送", nil);
                hud.mode = MBProgressHUDModeCustomView;
                [strongSelf performSelector:@selector(finish) withObject:weakSelf afterDelay:1];
            }
            else if (error.code == 12) {
                hud.minSize = CGSizeZero;
                hud.mode = MBProgressHUDModeText;
                hud.label.text = NSLocalizedString(@"已添加过该设备", nil);
                [strongSelf performSelector:@selector(finish) withObject:weakSelf afterDelay:1];
            }
            else {
                hud.minSize = CGSizeZero;
                hud.mode = MBProgressHUDModeText;
                hud.label.text = NSLocalizedString(@"验证设备失败", nil);
                [hud hideAnimated:YES afterDelay:1];
                [strongSelf startScan];
            }
        });
    });
}

-(void)finish
{
    [self.navigationController popViewControllerAnimated:YES];
}

@end
