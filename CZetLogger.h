#pragma once

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <thread>

#ifdef __GNUC__
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#endif


#include "CZetLoggerVar.h"
#include "CPartialLogger.h"
#include "CFileWritter.h"


class CZetLogger{

	static std::string path;

	std::string logger_name;

	std::thread *thread;
	bool end_loop_mdb;

	bool header_written;
	time_t m_CreatedTime;

	typedef struct{
		char 			m_lastOutput[1024];
		CPartialLogger *m_plogger;
	}tInfoPartialLogger;

	std::vector<tInfoPartialLogger > m_list;


	std::string m_currentOutPut;


	char m_delimiter;

	CFileWritter *m_fileWritter;
	std::string write_path;

	void basicSetup();

	void update();
	static void mainLoop(CZetLogger *);

public:

	enum{
		CVS_FORMAT=0x1,
	};

	static void setPathWrite(const std::string & _path);
	CZetLogger();
	CZetLogger(const std::string & path);

	std::string getName();
	void start();
	void stop();

	/**
	 * Sets delimiter between columns
	 */
	void setDelimiter(char delimiter);

	/**
	 * Add a partial logger already created.
	 */
	bool addPartialLogger(CPartialLogger *m_partial);

	~CZetLogger();

};
