//
//  ContentView.swift
//  Vision Playground
//
//  Created by Kai Oezer on 19.11.20.
//  Copyright © 2020 Kai Oezer. All rights reserved.
//

import SwiftUI

struct MainView: View
{
	var body: some View
	{
		TabView {

			FeatureLabView()
				.frame(maxWidth: .infinity, maxHeight: .infinity)
				.tabItem { Text("Feature Lab") }

		}
		.padding(8)
	}
}


struct MainView_Previews: PreviewProvider
{
	static var previews: some View {
		MainView()
	}
}
