//
//  NoteSpace.h
//
//  Created by Tim Murray-Browne on 24/01/2013.
//
//

/// A generic interface for classes that use the openframeworks/cinder style update/draw mechanics

#pragma once

namespace tmb
{
	class Module
	{
	public:
		virtual void update(float dt, float elapsedTime) {}
		virtual void draw() {}
	};
}