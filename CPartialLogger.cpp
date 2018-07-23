#include "CZetLogger.h"

CPartialLogger::CPartialLogger(){
	m_logger = NULL;
	n_write = n_read = 0;
}

CZetLogger *CPartialLogger::getLogger(){
	return m_logger;
}

void 	 CPartialLogger::setLogger(CZetLogger *logger){
	m_logger = logger;
}

void CPartialLogger::addVariable(CZetLoggerVar *var){
	m_inVariable.push_back(var);
}

std::vector<CZetLoggerVar *> * CPartialLogger::getListVariables(){
	return &m_inVariable;
}

void CPartialLogger::clearVars(){
	m_inVariable.clear();
}

void CPartialLogger::flushVarValues(){

	if(((n_write+1)& MASK_N_PARTIAL_LOGGER_BLOCKS) != n_read){

		m_out[n_write].clear();
		for(unsigned i = 0; i < m_inVariable.size(); i++){

			std::string s = m_inVariable[i]->getValue();

			m_out[n_write].push_back(s);
		}
		n_write=(n_write+1)& MASK_N_PARTIAL_LOGGER_BLOCKS;
	}
}

void CPartialLogger::setString(const std::string & str){

	if(((n_write+1)& MASK_N_PARTIAL_LOGGER_BLOCKS) != n_read){

		m_out[n_write].clear();

		m_out[n_write].push_back(str);

		n_write=(n_write+1)& MASK_N_PARTIAL_LOGGER_BLOCKS;
	}
}
