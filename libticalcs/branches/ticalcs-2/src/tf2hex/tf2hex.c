#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define STEP	8

int main(int argc, char **argv)
{
	const char *src_name;
	const char *dst_name;
	FILE *fi, *fo;
	FILE *so = stdout;
	struct _stat st;
	long length;
	unsigned char data[65536];
	int i, j;
	const char *pat;

	if(argc != 4)
		return -1;

	src_name = argv[1];
	dst_name = argv[2];
	pat = argv[3];

	// read input file
	fi = fopen(src_name, "rb");
	if(fi == NULL)
	{
		fprintf(stderr, "Unable to open input file.\n");
		return -1;
	}

	_fstat(_fileno(fi), &st);
	length = st.st_size;

	fread(data, sizeof(char), length, fi);
	fprintf(stdout, "Read %i bytes.\n", length);

	fclose(fi);

	// write output file
	fo = fopen(dst_name, "wt");
	if(fi == NULL)
	{
		fprintf(stderr, "Unable to open output file.\n");
		return -1;
	}

	fprintf(fo, "static unsigned char romDump%s[] = {\n", pat);
	for(i = 0; i < length; i += STEP)
	{
		for(j = 0; (j < STEP) && (i+j < length); j++)
		{
			fprintf(fo, "0x%02x, ", data[i + j]);
			//fprintf(so, "0x%02x, ", data[i + j]);
		}
		fprintf(fo, "\n");
		//fprintf(so, "\n");
	}
	fprintf(fo, "};\nstatic int romDumpSize%s = %i;\n", pat, length);

	fclose(fi);
	fclose(fo);

	//while(1);

	return 0;
}