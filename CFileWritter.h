#pragma once


class CFileWritter{

	enum{
		BUFFER_LENGTH=4096,
		MASK_BUFFER_LENGTH=(BUFFER_LENGTH-1),
		NUM_BLOCKS=4,
		MASK_NUM_BLOCKS=(NUM_BLOCKS-1)
	};

	//------------------------------------------
	struct{
		char m_buffer[BUFFER_LENGTH];
		int	 n_bytes;
	}block[NUM_BLOCKS];

	std::thread *thread;

	int n_block_write;
	int n_block_read; // can write disk or socket (stream)...
	//------------------------------------------
	bool have_to_open;
	bool end_loop_mdb;



	std::string filename;
	FILE *fp;

	void update();
	static void mainLoop(CFileWritter *);

public:

	CFileWritter();

	void setFilename(const std::string & _filename);

	void start();
	void stop();

	bool write(const char * s, int length);

	~CFileWritter();

};
