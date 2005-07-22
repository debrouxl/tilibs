#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#define STEP	8

#define	FILENAME	"romdump.89z"

int main(int argc, char **argv)
{
	FILE *fi, *fo;
	FILE *so = stdout;
	struct _stat st;
	long length;
	unsigned char data[65536];
	int i, j;

	// read input file
	fi = fopen(FILENAME, "rb");
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
	fo = fopen("rom_hdr.h", "wt");
	if(fi == NULL)
	{
		fprintf(stderr, "Unable to open output file.\n");
		return -1;
	}

	fprintf(fo, "static unsigned char romDump89[] = {\n");
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
	fprintf(fo, "};\nstatic int romDumpSize89 = %i;\n", length);

	fclose(fi);
	fclose(fo);

	//while(1);

	return 0;
}