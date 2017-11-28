/*
* File:   Log.h
* Author: Alberto Lepe <dev@alepe.com>
*
* Created on December 1, 2015, 6:00 PM
*
* Expanded by Kamil Jacek on November 28, 2017, 2:00 PM
*/

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <time.h>
#include <string>
#include <windows.h>
#undef ERROR
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
		if(type == typelog::ERROR)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);	// Red
		else if(type == typelog::WARNING)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);	// Yellow
		else
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);	// White
		msglevel = type;
		operator << ("[" + getLabel(type) + "] - " + getCurrentTime());

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

	inline string getCurrentTime()
	{
		time_t t;
		time(&t);
		return ctime(&t);
	}
};

#endif  /* LOG_H */