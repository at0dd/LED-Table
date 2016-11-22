import Foundation
import UIKit

class Conversions {
    
    func stringToFloat(text: String)->CGFloat {
        return (CGFloat((text as NSString).floatValue))
    }
    
    func cgfloatToString(num: CGFloat)->String {
        let rounded = round(CGFloat(num))
        return String(format:"%.0f", rounded)
    }
    
    func isHexValue(hex: String)->Bool {
        let charset = "0123456789abcdefABCDEF#"
        for i in hex.characters {
            if (!charset.containsString(String(i))) {
                return false
            }
        }
        return true
    }
    
    func hexToRGB(hex: String)->Array<CGFloat> {
        var rgb:[CGFloat] = [-1, -1, -1]
        let stringRepresentation = hex
        var characters: Array<Character> = Array(count: 10, repeatedValue: "?")
        var counter = 0
        
        for character in (stringRepresentation.characters)
        {
            characters[counter] = character
            counter += 1
        }
        
        let isHex = isHexValue(hex)
        
        if (stringRepresentation.characters.count == 7 && isHex == true)
        {
            let red = (String(characters[1]) + String(characters[2])).uppercaseString
            let green = (String(characters[3]) + String(characters[4])).uppercaseString
            let blue = (String(characters[5]) + String(characters[6])).uppercaseString
            
            rgb[0] = CGFloat(UInt16(red, radix:16)!)
            rgb[1] = CGFloat(UInt(green, radix:16)!)
            rgb[2] = CGFloat(UInt(blue, radix:16)!)
        }
        
        return rgb
    }
    
    func rgbToHex(rgb: Array<CGFloat>)->String {
        let red = rgb[0]
        let green = rgb[1]
        let blue = rgb[2]
        
        let redValue:String
        let greenValue:String
        let blueValue:String
        
        if (red == 0) {
            redValue = String("00")
        } else {
            redValue = String(format:"%2X", Int(red))
        }
        if (green == 0) {
            greenValue = String("00")
        } else {
            greenValue = String(format:"%2X", Int(green))
        }
        if (blue == 0) {
            blueValue = String("00")
        } else {
            blueValue = String(format:"%2X", Int(blue))
        }
        
        let hexValue = ("#" + redValue + greenValue + blueValue)
        return hexValue
    }
}