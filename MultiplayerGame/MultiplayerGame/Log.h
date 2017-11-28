/*
* File:   Log.h
* Author: Alberto Lepe <dev@alepe.com>
*
* Created on December 1, 2015, 6:00 PM
*/

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>

using namespace std;

enum typelog {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

class LOG {
public:
	LOG() {}
	LOG(typelog type) {
		msglevel = type;
		operator << ("[" + getLabel(type) + "]");

	}
	~LOG() {
		if (opened) {
			cout << endl;
		}
		opened = false;
	}
	template<class T>
	LOG &operator<<(const T &msg) {
		cout << msg;
		cout << "\n";
		opened = true;

		return *this;
	}
private:
	bool opened = false;
	typelog msglevel = DEBUG;
	inline string getLabel(typelog type) {
		string label;
		switch (type) {
		case DEBUG: label = "DEBUG"; break;
		case INFO:  label = "INFO"; break;
		case WARNING:  label = "WARNING"; break;
		case ERROR: label = "ERROR"; break;
		}
		return label;
	}
};

#endif  /* LOG_H */