#include "CZetLogger.h"

void CFileWritter::mainLoop(CFileWritter *fw){
	fw->update();
}


CFileWritter::CFileWritter(){

	thread=NULL;
	n_block_write = n_block_read =0;
	memset(&block,0,sizeof(block));
	fp = NULL;
	have_to_open = true;
	end_loop_mdb=false;


}

void CFileWritter::update(){
	while(!end_loop_mdb)
	{

		// if file not opened, it doesn't have sense to write anything..
		if(fp ==NULL) return;

		if(((n_block_read) & MASK_NUM_BLOCKS) != n_block_write){

			if(((n_block_read+1) & MASK_NUM_BLOCKS) != n_block_write){
				//---- ready to write :)
				int n = fwrite(block[n_block_read].m_buffer,1,BUFFER_LENGTH,fp);

				if(n != BUFFER_LENGTH){ // something wrong happened!
					fprintf(stderr,"\nko!");
				}

				n_block_read= (n_block_read+1)&MASK_NUM_BLOCKS;
			}
		}
	}


}

void  CFileWritter::start(){
	if(thread!=NULL){
		return;
	}

	end_loop_mdb=false;
	thread = new std::thread(mainLoop,this);//mainLoop(this));
}

void  CFileWritter::stop(){
	if(thread != NULL){
		end_loop_mdb=true;
		thread->join();

		delete thread;
		thread=NULL;
	}
}

void CFileWritter::setFilename(const std::string & _filename){
	 filename = _filename;
}

bool CFileWritter::write(const char * s, int length){


	if(have_to_open){
		have_to_open=false;

		if((fp = fopen(filename.c_str(), "wt+")) == NULL){
			fprintf(stderr,"Cannot create file \"%s\".",filename.c_str());
			return false;
		}

	}


	if(fp ==NULL ) return false;


	if(((n_block_write+1)& MASK_NUM_BLOCKS) != n_block_read){

		int to_fill=length;//sffmpeg->audioFrame->nb_samples*sffmpeg->audioFrame->channels*2;// < to_fill)
		if(block[n_block_write].n_bytes+to_fill >= BUFFER_LENGTH){
			to_fill = BUFFER_LENGTH-block[n_block_write].n_bytes;
		}



		memcpy(&block[n_block_write].m_buffer[block[n_block_write].n_bytes],s, to_fill);

		//printf("\n%i/%i %i %i",block[n_block_write].n_bytes,BUFFER_LENGTH,n_block_write, n_block_read);

		//printf(" Block %2i %3i/%03i bytes %i to fill, %10i bytes original %3i len current block %3i %i ms ", n_blocks_write_memory,SPLAYER_FRAME_SIZE,sffmpeg->m_len[n_blocks_write_memory],to_fill,current_bytes_original+=to_fill,sffmpeg->audioFrame->nb_samples*sffmpeg->audioFrame->channels*2,sffmpeg->m_len[n_blocks_write_memory],(int)(current_bytes_original*ms)/4);

		block[n_block_write].n_bytes+=to_fill;

		if(block[n_block_write].n_bytes==BUFFER_LENGTH){ // next block

			int rest_fill = length - to_fill;
			n_block_write= (n_block_write+1)&MASK_NUM_BLOCKS;



			block[n_block_write].n_bytes=0;
			if(rest_fill>0){
				block[n_block_write].n_bytes=rest_fill;
				//printf("*write block %2i %3i/%03i bytes %i", n_blocks_write_memory,SPLAYER_FRAME_SIZE,sffmpeg->m_len[n_blocks_write_memory],rest_fill);
				memcpy(block[n_block_write].m_buffer,&s[to_fill], rest_fill);

			}
			//printf("*Block %2i %3i/%03i %10i bytes %i ms.", n_blocks_write_memory,SPLAYER_FRAME_SIZE,sffmpeg->m_len[n_blocks_write_memory],current_bytes_original+=rest_fill,(int)(current_bytes_original*ms)/4);
		}

		return true;
	}

	return false;
}

CFileWritter::~CFileWritter(){

	stop();

	if(fp != NULL){

		if(((n_block_write+1)& MASK_NUM_BLOCKS) != n_block_read)
			if(block[n_block_write].n_bytes > 0)
				fwrite(block[n_block_write].m_buffer,1,block[n_block_write].n_bytes,fp);

		fflush(fp);
		fclose(fp);
		have_to_open = true;
		fp=NULL;
	}
}

