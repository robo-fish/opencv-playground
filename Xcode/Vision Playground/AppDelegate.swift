//
//  AppDelegate.swift
//  Vision Playground
//
//  Created by Kai Oezer on 19.11.20.
//  Copyright © 2020 Kai Oezer. All rights reserved.
//

import Cocoa
import SwiftUI

@main
class AppDelegate: NSObject, NSApplicationDelegate
{
	var window : NSWindow?

	func applicationDidFinishLaunching(_ aNotification: Notification)
	{
		let w = NSWindow(
			contentRect: NSRect(x: 0, y: 0, width: 800, height: 600),
			styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
			backing: .buffered, defer: false)
		w.isReleasedWhenClosed = false
		w.center()
		w.setFrameAutosaveName("Main Window")
		w.contentView = NSHostingView(rootView: MainView())
		self.window = w
		w.makeKeyAndOrderFront(nil)
	}

	func applicationWillTerminate(_ aNotification: Notification)
	{
	}
}
