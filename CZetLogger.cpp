
#include "CZetLogger.h"




std::string CZetLogger::path=".";



void CZetLogger::mainLoop(CZetLogger *zl){
	zl->update();
}


bool isDirectory(const std::string & filename){
	int status;
	struct stat st_buf;
	//int ret_stat;

	status = stat (filename.c_str(), &st_buf);
	    if (status != 0) {
	        //print_error_cr ("Error, errno = %d\n", errno);
	        return false;
	    }


	return S_ISDIR (st_buf.st_mode) != 0;


}

void CZetLogger::setPathWrite(const std::string & _path){
	if(isDirectory(_path)){

		path = _path;
#ifdef __ZETLOGGER_DEBUG__
		print_info_cr("* Set log write path at %s",path.c_str());
#endif
	}else{

		fprintf(stderr,"Cannot set log path at %s because is not directory",_path.c_str());

	}
}




void CZetLogger::basicSetup(){

	header_written = false;
	m_fileWritter = NULL;
	m_CreatedTime=0;
	end_loop_mdb=false;




	m_currentOutPut="";
	m_delimiter=';';
	write_path = CZetLogger::path;

	logger_name="";
}

CZetLogger::CZetLogger(){
	basicSetup();
}

CZetLogger::CZetLogger(const std::string & path){
	basicSetup();
	write_path = path;

}

//--------------------------------------------------------------------
void  CZetLogger::update()  //  Receive  messages,  gest  &  send...
{
	while(!end_loop_mdb)
	{
		char aux[1024]={0};
		char str_del[2]={m_delimiter,0};
		bool updated=false;




		for(unsigned i = 0; i < m_list.size(); i++){
			CPartialLogger *pl = m_list[i].m_plogger;
			if(((pl->n_read) & MASK_N_PARTIAL_LOGGER_BLOCKS) != pl->n_write){

				//if(((pl->n_read+1) & MASK_N_PARTIAL_LOGGER_BLOCKS) != pl->n_write){
					std::vector<std::string> *m_out = &pl->m_out[pl->n_read];


					updated=true;
					//if(i>0)
					//	strcat(aux,str_del);
					memset(m_list[i].m_lastOutput, 0, sizeof(m_list[i].m_lastOutput));

					// set block to update...
					for(unsigned j = 0; j < m_out->size(); j++){
						if(j>0)
							strcat(m_list[i].m_lastOutput,str_del);
						strcat(m_list[i].m_lastOutput,(char *)m_out->at(j).c_str());
					}

					if(i>0)
						strcat(aux,str_del);
					strcat(aux, m_list[i].m_lastOutput);


					pl->n_read = (pl->n_read+1)& MASK_N_PARTIAL_LOGGER_BLOCKS;
				//}
			}

			else{
				/*if(m_list[i].m_lastOutput[0] == 0) // is not written yet --> no update...
					updated&=false;
				else*/
				{
					if(i>0)
						strcat(aux,str_del);
					strcat(aux, m_list[i].m_lastOutput);
				}
			}
		}

		if(updated) {

			//printf("\nu %s ",aux);

			if(!header_written){ // time to write the header...
				char m_cp_header[4096]={0};
				char m_columns[2048]={0};
				/*char m_eyetracker_name[1024]={"Unknow tracker"};
				if(CEyeTracker::getCurrentTracker()){
					strcpy(m_eyetracker_name,CEyeTracker::getCurrentTracker()->getName().c_str());
				}*/

				for(unsigned i = 0; i < m_list.size(); i++){
					std::vector<CZetLoggerVar *> *l_obj = m_list[i].m_plogger->getListVariables();

					if(i > 0){
						strcat(m_columns,str_del);
					}

					for(unsigned j = 0; j  < l_obj->size(); j++){
						if(j > 0)
							strcat(m_columns,str_del);
						strcat(m_columns,l_obj->at(j)->getName().c_str());
					}
				}

				struct tm *curtime = localtime ( &m_CreatedTime );


				sprintf(m_cp_header,"Date and time: %4i.%02i.%02i %2i:%02i"
									//"\nTracker: %s"
									"\n%s\n",
									curtime->tm_year+1900,
									curtime->tm_mon+1,
									curtime->tm_mday,
									curtime->tm_hour,
									curtime->tm_min,

										//m_eyetracker_name,
									m_columns);
				// write columns ...

				//strcat(m_cp_header,"\n");
				m_fileWritter->write(m_cp_header,strlen(m_cp_header));
				header_written = true;
			}

			strcat(aux,"\n");
			m_fileWritter->write(aux,strlen(aux));
		}

	}

}


void CZetLogger::start(){



	if(m_fileWritter==NULL){

		char buf[1024];
		 m_CreatedTime =time(NULL );
		struct tm *curtime = localtime ( &m_CreatedTime );

		sprintf(buf,"%s/%s_%4i_%02i_%02i_%02i_%02i.txt",write_path.c_str(),logger_name.c_str(),curtime->tm_year+1900,curtime->tm_mon+1,curtime->tm_mday,curtime->tm_hour,curtime->tm_min);

		m_fileWritter=new CFileWritter();

		m_fileWritter->setFilename(buf);
	}

	//CThread::start();	// start writter...
	if(thread!=NULL){
			return;
	}

	end_loop_mdb=false;
	thread = new std::thread(mainLoop,this);//mainLoop(this));

	m_fileWritter->start(); // start reader ..
#ifdef __ZETLOGGER_DEBUG__
	printf("log \"%s\" started\n",m_id.c_str());
#endif
}

void CZetLogger::stop()
{
	if(thread == NULL){ // already stoped...
		return;
	}

	//CThread::stop();
	end_loop_mdb=true;
	thread->join();

	delete thread;
	thread=NULL;



	if(m_fileWritter!=NULL)
		m_fileWritter->stop(); //stops reader...
#ifdef __ZETLOGGER_DEBUG__
	printf("log  stopped\n");
#endif
}

std::string CZetLogger::getName(){
	return logger_name;
}

void CZetLogger::setDelimiter(char delimiter){
	m_delimiter = delimiter;
}


bool CZetLogger::addPartialLogger(CPartialLogger *m_partial){
	if(m_partial == NULL) {
		return false;
	}

	if(m_partial->getLogger() != NULL) {
		fprintf(stderr,"Error partial logger has been already parented with \"%s\"!",m_partial->getLogger()->getName().c_str());
		return false;
	}


	tInfoPartialLogger info;

	info.m_plogger = m_partial;
	memset(info.m_lastOutput,0,sizeof(info.m_lastOutput));

	m_list.push_back(info);
	m_partial->setLogger(this);

	return true;
}

CZetLogger::~CZetLogger(){
	CZetLogger::stop();

	for(unsigned int i = 0; i < m_list.size(); i++){ // reset parent logger's
		m_list[i].m_plogger->setLogger(NULL);
	}

//	m_fileWritter->stop();
	delete m_fileWritter;

	m_fileWritter = NULL;
}
