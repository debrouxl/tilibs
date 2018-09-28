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
	struct stat st;
	long length, lenread;
	static unsigned char data[65536];
	int i, j;
	const char *pat;

	if(argc != 4)
	{
		fprintf(stderr, "Usage: tf2hex input_file output_file suffix\n");
		return 1;
	}

	src_name = argv[1];
	dst_name = argv[2];
	pat = argv[3];

	// read input file
	fi = fopen(src_name, "rb");
	if(fi == NULL)
	{
		fprintf(stderr, "Unable to open input file.\n");
		return 1;
	}

	fstat(fileno(fi), &st);
	length = st.st_size;

	lenread = fread(data, sizeof(char), length, fi);
	fprintf(stdout, "Read %ld bytes.\n", lenread);

	fclose(fi);

	if (lenread == length)
	{
		// write output file
		fo = fopen(dst_name, "wt");
		if(fo == NULL)
		{
			fprintf(stderr, "Unable to open output file.\n");
			return 1;
		}

		fprintf(fo, "static unsigned char romDump%s[] = {\n", pat);
		for(i = 0; i < length; i += STEP)
		{
			for(j = 0; (j < STEP) && (i+j < length); j++)
			{
				fprintf(fo, "0x%02x, ", data[i + j]);
			}
			fprintf(fo, "\n");
		}
		fprintf(fo, "};\nstatic unsigned int romDumpSize%s = %ld;\n", pat, length);

		fclose(fo);
	}
	else
	{
		fprintf(stderr, "Was unable to read the entire input file.\n");
		return 1;
	}

	return 0;
}
