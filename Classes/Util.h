#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <sstream>

template <class T>
std::string toStr(T val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

template <class T>
T strTo(const std::string& val)
{
	T ret;
	std::stringstream ss(val);
	ss >> ret;
	return ret;
}

std::string getRandom4CardsString();

#endif // __UTIL_H__