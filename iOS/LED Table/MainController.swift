//
//  MainController.swift
//  LED Table
//
//  Created by Alex Todd on 12/1/15.
//  Copyright © 2015 Alex Todd. All rights reserved.
//

import UIKit

var connected = false
var condition = 0

class MainController: UIViewController, BLEDelegate {
    var ble = BLE()
    var convert = Conversions()
    
    @IBOutlet weak var uuidLabel: UILabel!
    @IBOutlet weak var optionsButton: UIButton!
    @IBOutlet weak var redSlider: UISlider!
    @IBOutlet weak var greenSlider: UISlider!
    @IBOutlet weak var blueSlider: UISlider!
    @IBOutlet weak var redRGB: UITextField!
    @IBOutlet weak var greenRGB: UITextField!
    @IBOutlet weak var blueRGB: UITextField!
    @IBOutlet weak var hexValue: UITextField!
    @IBOutlet weak var c1Btn: UIButton!
    @IBOutlet weak var c2Btn: UIButton!
    @IBOutlet weak var c3Btn: UIButton!
    @IBOutlet weak var c4Btn: UIButton!
    @IBOutlet weak var c5Btn: UIButton!
    @IBOutlet weak var c6Btn: UIButton!
    @IBOutlet weak var c7Btn: UIButton!
    @IBOutlet weak var treeBtn: UIButton!
    
    func enableColors() {
        redSlider.enabled = true
        greenSlider.enabled = true
        blueSlider.enabled = true
        redRGB.enabled = true
        greenRGB.enabled = true
        blueRGB.enabled = true
        hexValue.enabled = true
        return
    }
    
    func disableColors() {
        redSlider.enabled = false
        greenSlider.enabled = false
        blueSlider.enabled = false
        redRGB.enabled = false
        greenRGB.enabled = false
        blueRGB.enabled = false
        hexValue.enabled = false
        return
    }
    
    func enableGame() {
        c1Btn.enabled = true
        c2Btn.enabled = true
        c3Btn.enabled = true
        c4Btn.enabled = true
        c5Btn.enabled = true
        c6Btn.enabled = true
        c7Btn.enabled = true
        return
    }
    
    func disableGame() {
        c1Btn.enabled = false
        c2Btn.enabled = false
        c3Btn.enabled = false
        c4Btn.enabled = false
        c5Btn.enabled = false
        c6Btn.enabled = false
        c7Btn.enabled = false
        return
    }
    
    
    @IBAction func solidColorBtn(sender: AnyObject) {
        condition = 0
        let data = NSData(bytes: [UInt8(condition), UInt8(Int(red)), UInt8(Int(green)), UInt8(Int(blue))] as [UInt8], length: 4)
        self.ble.write(data: data)
        enableColors()
    }
    
    @IBAction func solidChaseBtn(sender: AnyObject) {
        condition = 1
        let data = NSData(bytes: [UInt8(condition), UInt8(Int(red)), UInt8(Int(green)), UInt8(Int(blue))] as [UInt8], length: 4)
        self.ble.write(data: data)
        enableColors()
    }
    
    @IBAction func rainbowBtn(sender: AnyObject) {
        condition = 2
        let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
        self.ble.write(data: data)
        disableColors()
    }
    
    @IBAction func rainbowChaseBtn(sender: AnyObject) {
        condition = 3
        let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
        self.ble.write(data: data)
        disableColors()
    }
    
    @IBAction func connectFourBtn(sender: AnyObject){
        condition = 4
        let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
        self.ble.write(data: data)
        disableColors()
        
    }
    
    @IBAction func wildcatBtn(sender: AnyObject) {
        condition = 5
        let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
        self.ble.write(data: data)
        disableColors()
    }
    
    @IBAction func treeBtn(sender: AnyObject) {
        condition = 6
        let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
        self.ble.write(data: data)
        disableColors()
    }
    
    var clicks = 0
    
    @IBAction func c1(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(10)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    @IBAction func c2(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(11)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    @IBAction func c3(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(12)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    @IBAction func c4(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(13)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    @IBAction func c5(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(14)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    @IBAction func c6(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(15)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    @IBAction func c7(sender: AnyObject) {
        clicks += 1
        let data = NSData(bytes: [UInt8(16)] as [UInt8], length: 1)
        self.ble.write(data: data)
    }
    
    var red:CGFloat = 0
    var green:CGFloat = 0
    var blue:CGFloat = 0
    var devInfo = true
    let tapInfo = UITapGestureRecognizer()

    override func viewDidLoad() {
        super.viewDidLoad()
        ble.delegate = self
        uuidLabel.addGestureRecognizer(tapInfo)
        tapInfo.addTarget(self, action: #selector(MainController.updateDevInfo))
        
        //Create Refresh Button
        let newButton = UIBarButtonItem(barButtonSystemItem: .Refresh, target: self, action: #selector(MainController.connectButtonPressed(_:)))
        self.navigationItem.rightBarButtonItem = newButton
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
    
    
    /*override func viewDidAppear(animated: Bool) {
        let r : Int = Int(red)
        let g : Int = Int(green)
        let b : Int = Int(blue)
        switch(condition) {
        case 0:
            enableColors()
            let data = NSData(bytes: [UInt8(condition), UInt8(r), UInt8(g), UInt8(b)] as [UInt8], length: 4)
            self.ble.write(data: data)
            break
        case 1:
            enableColors()
            let data = NSData(bytes: [UInt8(condition), UInt8(r), UInt8(g), UInt8(b)] as [UInt8], length: 4)
            self.ble.write(data: data)
            break
        case 2:
            disableColors()
            let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
            self.ble.write(data: data)
            break
        case 3:
            disableColors()
            let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
            self.ble.write(data: data)
            break
        case 4:
            disableColors()
            let data = NSData(bytes: [UInt8(condition)] as [UInt8], length: 1)
            self.ble.write(data: data)
        default:
            break
        }
    }
    */
    
    /* --------- Bluetooth Functions ------- */
    @IBAction func connectButtonPressed(sender: UIBarButtonItem) {
        self.navigationItem.rightBarButtonItem?.enabled = false
        if connected {
            self.ble.disconnectFromPeripheral(ble.peripherals[0])
            //self.optionsButton.enabled = false
            self.navigationItem.rightBarButtonItem?.enabled = true
            return
        }
        self.ble.startScanning(5)
        self.navigationItem.rightBarButtonItem?.enabled = false
        NSTimer.scheduledTimerWithTimeInterval(5, target: self, selector: #selector(MainController.connect), userInfo: nil, repeats: false)
    }
    
    func connect() {
        if (connected == false) {
            if (ble.peripherals.count > 0) {
                self.ble.connectToPeripheral(ble.peripherals[0])
                //self.optionsButton.enabled = true
                return
            }
        }
        if (connected == false) {
            let alertController = UIAlertController(title: "Error", message: "No devices found!", preferredStyle: .Alert);
            let OKAction = UIAlertAction(title: "OK", style: .Default) {(action) in}
            alertController.addAction(OKAction)
            self.presentViewController(alertController, animated: true) {}
            self.navigationItem.rightBarButtonItem?.enabled = true
        }
    }
        
    func bleDidUpdateState(state: BLEState) {
        if state == .PoweredOn {
            self.ble.startScanning(1)
        }
    }
    
    func updateDevInfo() {
        if (devInfo == false){
            uuidLabel.text = ble.uuidLabel!
            devInfo = true
        } else {
            uuidLabel.text = "Connected to \(ble.peripherals[0].name!)"
            devInfo = false
        }
    }
    
    func bleDidConnectToPeripheral() {
        connected = true
        updateDevInfo()
        enableColors()
        //optionsButton.enabled = true
        let newButton = UIBarButtonItem(barButtonSystemItem: .Stop, target: self, action: #selector(MainController.connectButtonPressed(_:)))
        self.navigationItem.rightBarButtonItem = newButton
        self.navigationItem.rightBarButtonItem?.enabled = true
    }

    func bleDidDisconenctFromPeripheral() {
        connected = false
        uuidLabel.text = ""
        disableColors()
        //optionsButton.enabled = false
        let newButton = UIBarButtonItem(barButtonSystemItem: .Refresh, target: self, action: #selector(MainController.connectButtonPressed(_:)))
        self.navigationItem.rightBarButtonItem = newButton
        let alertController = UIAlertController(title: "Disconnected", message: "The bluetooth device has been disconnected.", preferredStyle: .Alert);
        let OKAction = UIAlertAction(title: "OK", style: .Default) {(action) in}
        alertController.addAction(OKAction)
        self.presentViewController(alertController, animated: true) {}
        self.navigationItem.rightBarButtonItem?.enabled = true
    }
    
    func bleDidReceiveData(data: NSData?) {
    }
    
    /* --------- Solid Color Functions ------- */
    @IBAction func updatedSlider() {
        red = CGFloat(redSlider.value)
        green = CGFloat(greenSlider.value)
        blue = CGFloat(blueSlider.value)
        updateAll()
    }
    
    @IBAction func updatedRGB() {
        red = convert.stringToFloat(redRGB.text!)
        green = convert.stringToFloat(greenRGB.text!)
        blue = convert.stringToFloat(blueRGB.text!)
        if (red < 0 || red > 256 || green < 0 || green > 256 || blue < 0 || blue > 256) {
            let alertController = UIAlertController(title: "Error", message: "Please enter a valid RGB value! (0-255)", preferredStyle: .Alert);
            let OKAction = UIAlertAction(title: "OK", style: .Default) {(action) in}
            alertController.addAction(OKAction)
            self.presentViewController(alertController, animated: true) {}
            return
        }
        updateAll()
    }
    
    @IBAction func updatedHex() {
        let rgb = convert.hexToRGB(hexValue.text!)
        
        if (rgb.contains(-1)) {
            let alertController = UIAlertController(title: "Error", message: "Please enter a valid hexadecimal value starting with #!", preferredStyle: .Alert);
            let OKAction = UIAlertAction(title: "OK", style: .Default) {(action) in}
            alertController.addAction(OKAction)
            self.presentViewController(alertController, animated: true) {}
            return
        }
        
        red = rgb[0]
        green = rgb[1]
        blue = rgb[2]
        updateAll()
    }
    
    func updateAll() {
        redSlider.value = Float(red)
        greenSlider.value = Float(green)
        blueSlider.value = Float(blue)
        
        redRGB.text = convert.cgfloatToString(red)
        greenRGB.text = convert.cgfloatToString(green)
        blueRGB.text = convert.cgfloatToString(blue)
        
        hexValue.text = convert.rgbToHex([red, green, blue])
        
        let r : Int = Int(red)
        let g : Int = Int(green)
        let b : Int = Int(blue)
        let data = NSData(bytes: [UInt8(condition), UInt8(r), UInt8(g), UInt8(b)] as [UInt8], length: 4)
        self.ble.write(data: data)
    }
}