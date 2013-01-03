//
//  Utilities.h
//
//  Created by Tim Murray-Browne on 03/01/2013.
//
//

#pragma once

#include <string>
#include <stringstream>

namespace tmb
{
	template <typename T>
	std::string toString(T const& x)
	{
		std::stringstream s;
		s << x;
		return s.str();
	}
	
	
	
	
}