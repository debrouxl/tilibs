#include <stdio.h>
#include <glib.h>
#include <string.h>

#define TOKEN1	"==========[ List of entries ]=========="
#define TOKEN2	"==========[end of list]=========="

GList *topics = NULL;

typedef struct
{
	gchar*	filename;
	gchar*	title;
} topic;

int main(int argc, char **argv)
{
	char *folder;
	char *txt_file;
	char *src_file;
	char *dst_file;
	FILE *f;
	FILE *fi, *fo;
	char line[65536];
	int skip = 0;

	// Check and get program arguments
#if 0
	if(argc < 2)
		return -1;

	folder = argv[1];
#else
	// test
	folder = "C:\\sources\\roms\\tifiles-2\\docs\\";
#endif
	printf("Base folder: <%s>\n", folder);

	// Open list of topics ("api.txt")
	txt_file = g_strconcat(folder, G_DIR_SEPARATOR_S, "api", NULL);
	f = fopen(txt_file, "rt");
	if(f == NULL)
	{
		printf("Can't open list of topics: <%s>.\n", txt_file);
		return -1;
	}

	// Read list of topics
	while(!feof(f))
	{
		gchar **str_array;
		topic *t;

		fgets(line, sizeof(line), f);
		str_array = g_strsplit(line, ":", 2);

		t = g_malloc0(sizeof(topic));
		t->filename = str_array[0];
		t->title = str_array[1];

		topics = g_list_append(topics, t);
	}

	// Close file
	fclose(f);

	//===

	// Open "api.html" file
	src_file = g_strconcat(folder, G_DIR_SEPARATOR_S, "\\tmpl\\api.html", NULL);
	fi = fopen(src_file, "rt");
	if(fi == NULL)
	{
		printf("Can't open input file: <%s>.\n", txt_file);
		return -1;
	}

	dst_file = g_strconcat(folder, G_DIR_SEPARATOR_S, "\\html\\api.html", NULL);
	fo = fopen(dst_file, "wt");
	if(fo == NULL)
	{
		printf("Can't open output file: <%s>.\n", dst_file);
		return -1;
	}

	// Search for list of topics
	while(!feof(fi))
	{
		fgets(line, sizeof(line), fi);

		if(strstr(line, TOKEN1))
			skip = !0;

		if(!skip)
			fputs(line, fo);

		if(strstr(line, TOKEN2))
			skip = 0;
	}

	// Close files
	fclose(fi);
	fclose(fo);
	
	
	return 0;
}