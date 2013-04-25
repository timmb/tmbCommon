//
//  Utilities.h
//
//  Created by Tim Murray-Browne on 03/01/2013.
//
//

#pragma once

#include <string>
#include <sstream>

#define FILESTRING____ (std::string(__FILE__).substr(std::string(__FILE__).find_last_of('/')))
#define LOG(str) std::cout << "LOG: " << FILESTRING____ << ':' << __LINE__ << ' ' << __func__ << ": " << str << std::endl;
#define ERR(str) std::cerr << "ERR: " << FILESTRING____ << ':' << __LINE__ << ' ' << __func__ << ": " << str << std::endl;

namespace tmb
{
	/// Convert a streamable type to a string
	template <typename T>
	std::string toString(T const& x)
	{
		std::stringstream s;
		s << std::fixed;
		s << x;
		return s.str();
	}
	
#ifndef PI
	static const double PI = (3.14159265358979323846264338327950288);
#endif
	
	
	static std::string midiNoteName(int midiNote)
	{
		if (midiNote<0 || midiNote>=128)
			return "?";

		const std::string notes[12] = { "C","C#","D","D#","E","F","F#","G","G#","A","A#","B" };
		const std::string octaves[11] =
		{ "0","1","2","3","4","5","6","7","8","9", "10" };
		return notes[midiNote%12] + octaves[midiNote/12];
	}


	template <typename T>
	T toDegrees(T const& radians)
{
	return radians * 360. / (2. * PI);
}
	
}