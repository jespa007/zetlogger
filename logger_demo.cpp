#include "CZetLogger.h"

int main(int argc, char *argv[])
{

	CZetLogger *logger = new CZetLogger();


	logger->start();

	do{

	}while(getchar()!='s');

	printf("shutdown...\n");

	delete logger;

	return 0;
}
