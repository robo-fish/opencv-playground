//
//  FeatureLabView.swift
//  Vision Playground
//
//  Created by Kai Oezer on 19.11.20.
//  Copyright © 2020 Kai Oezer. All rights reserved.
//

import SwiftUI
import AppKit

struct FeatureLabView : NSViewControllerRepresentable
{
	func makeNSViewController(context: Self.Context) -> FLViewController
	{
		let storyboard = NSStoryboard(name:"FeatureLab", bundle: Bundle.main)
		let controller = storyboard.instantiateInitialController() as? FLViewController
		return controller ?? FLViewController()
	}

	func updateNSViewController(_ nsViewController: FLViewController, context: Context)
	{

	}
}
