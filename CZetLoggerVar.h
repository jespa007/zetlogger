#pragma once

class CZetLoggerVar{
	std::string value, name;
public:
	std::string getValue(){return value;};
	std::string getName(){return name;};

	~CZetLoggerVar(){};
};
