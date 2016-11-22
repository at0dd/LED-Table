//
//  OptionsController.swift
//  LED Table
//
//  Created by Alex Todd on 12/3/15.
//  Copyright © 2015 Alex Todd. All rights reserved.
//

import UIKit

class OptionsController: UITableViewController {
    
    //Presets
    @IBOutlet weak var solidColorCell: UITableViewCell!
    @IBOutlet weak var solidColorChaseCell: UITableViewCell!
    @IBOutlet weak var rainbowCell: UITableViewCell!
    @IBOutlet weak var rainbowChaseCell: UITableViewCell!
    @IBOutlet weak var wildcatCell: UITableViewCell!
    
    override func tableView(tableView: UITableView, didSelectRowAtIndexPath indexPath: NSIndexPath) {
        switch(indexPath.section)
        {
        case 0: //Presets
            switch(indexPath.row){
            case 0:
                condition = 0
                navigationController?.popToRootViewControllerAnimated(true)
                break
            case 1:
                condition = 1
                navigationController?.popToRootViewControllerAnimated(true)
                break
            case 2:
                condition = 2
                navigationController?.popToRootViewControllerAnimated(true)
                break
            case 3:
                condition = 3
                navigationController?.popToRootViewControllerAnimated(true)
                break
            case 4:
                condition = 4
                navigationController?.popToRootViewControllerAnimated(true)
                break
            default:
                break
            }
        default:
            break
        }
    }
    
    func updateChecks() {
        solidColorCell.accessoryType = .None
        solidColorChaseCell.accessoryType = .None
        rainbowCell.accessoryType = .None
        rainbowChaseCell.accessoryType = .None
        wildcatCell.accessoryType = .None
        
        switch(condition) {
        case 0:
            solidColorCell.accessoryType = .Checkmark
            break
        case 1:
            solidColorChaseCell.accessoryType = .Checkmark
            break
        case 2:
            rainbowCell.accessoryType = .Checkmark
            break
        case 3:
            rainbowChaseCell.accessoryType = .Checkmark
            break
        case 4:
            wildcatCell.accessoryType = .Checkmark
            break
        default:
            break
        }

    }
    
    override func viewDidLoad() {
        super.viewDidLoad()
        updateChecks()
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
    }
}