/*
*	Text processing utilities
*/
#pragma once
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
//#include <iostream> //for cout debugging

// trim from start - only allow characters that have a graphical representation
//TODO: could have a separate function for a whitespace trim - this is whitespace+control
//Basically, we don't want the SOH and ETX characters to remain
//NOTE: isspace is 0x20, \t (0x09), \n (0x0a), \v (0x0b), \f (0x0c), \r (0x0d)
//isspace, isgraph, isprint, iscntrl
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

//trim from start, any character that doesn't have a graphical representation
static inline std::string &ltrim_all(std::string &s) {
		//isgraph is any character with a graphical representation (space and tab don't)
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::ptr_fun<int, int>(std::isgraph)));
        return s;
}

// trim from end, any character that doesn't have a graphical representation
static inline std::string &rtrim_all(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::ptr_fun<int, int>(std::isgraph)).base(), s.end());
        return s;
}

// trim from both ends, any character that doesn't have a graphical representation
static inline std::string &trim_all(std::string &s) {
        return ltrim_all(rtrim_all(s));
}

//#define SPACES " \t\r\n"
//
//inline string trim_right(const string& s, const string& t = SPACES)
//{ 
//	string d (s);
//	string::size_type i (d.find_last_not_of (t));
//	if (i == string::npos)
//		return "";
//	else
//		return d.erase (d.find_last_not_of (t) + 1);
//}
//
//inline string trim_left(const string& s, const string& t = SPACES)
//{
//	string d (s);
//	return d.erase (0, s.find_first_not_of (t));
//}
//
//inline string trim(const string& s, const string& t = SPACES)
//{
//	string d (s);
//	return trim_left(trim_right (d, t), t);
//}


//TODO: don't confuse with toupper and tolower - really need new namespace
//convert to uppercase
static inline std::string &to_upper(std::string &s) {
	transform(s.begin(),s.end(),s.begin(),std::toupper);
	return s;
}

//convert to lowercase
static inline std::string &tolower(std::string &s) {
	//transform(s.begin(),s.end(),s.begin(),(int(*)(int))std::tolower);
	transform(s.begin(),s.end(),s.begin(),std::tolower);
	return s;
}