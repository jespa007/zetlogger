#pragma once

#define N_PARTIAL_LOGGER_BLOCKS				(16)
#define MASK_N_PARTIAL_LOGGER_BLOCKS		(16-1)
class CZetLogger;
class CPartialLogger{

	CZetLogger *m_logger;

	std::vector<CZetLoggerVar *> m_inVariable;

public:
	std::vector<std::string> m_out[N_PARTIAL_LOGGER_BLOCKS];
	unsigned char n_write, n_read;


	CPartialLogger();
	CZetLogger *getLogger();
	void setLogger(CZetLogger *logger);

	void addVariable(CZetLoggerVar *var);
	void clearVars();

	std::vector<CZetLoggerVar *> * getListVariables();

	/**
	 * Called in some point of our app to realise there're changes to log.
	 */
	void flushVarValues();
	void setString(const std::string & str);

};
