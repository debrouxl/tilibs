#include <stdio.h>
#include <glib.h>
#include <string.h>

#define TOKEN1	"==========[ List of entries ]=========="

GList *topics = NULL;

typedef struct
{
	gchar*	filename;
	gchar*  basename;
	gchar*	title;
	GList*  fncts;
} topic;

// Open a C or H file and search for comment on function.
static int get_list_of_functions(const char *filename, GList **fncts)
{
	FILE *f;
	char line[65536];
	
	f = fopen(filename, "rt");
	if(f == NULL)
	{
		printf("Can't open this file: <%s>\n", filename);
		return -1;
	}

	while(!feof(f))
	{
		fgets(line, sizeof(line), f);
		if(line[0] == '/' && line[1] == '*' && line[2] == '*' &&
			line[3] != '*')
		{
			char *d;
			gchar *s;

			fgets(line, sizeof(line), f);
			d = strrchr(line, ':');
			if(d != NULL)
				*d = '\0';
			
			s = g_strdup(line+3);
			//printf("[%s]\n", s);
			*fncts = g_list_append(*fncts, s);
		}
	}

	fclose(f);

	return 0;
}

static int write_api_toc(FILE *fo)
{
	GList *l, *m;
	
	// parse topics and functions
	for(l = topics; l != NULL; l = g_list_next(l))
	{
		topic *t = (topic *)l->data;
		GList *fncts = t->fncts;
		
		fprintf(fo, "<span style=\"font-weight:bold;\">%s</span><br>\n", t->title);
		fprintf(fo, "<ul>\n");
		for(m = fncts; m != NULL; m = g_list_next(m))
			fprintf(fo, "<li><a href=\"%s.htm#%s\">%s</a></li>\n", 
				t->basename, (char *)m->data, (char *)m->data);
		fprintf(fo, "</ul>\n");
	}

	return 0;
}

static int write_topic_header(FILE *f, const char *topic_name)
{
	fprintf(f, "<html>\n");
	fprintf(f, "<head>\n");
	fprintf(f, "<title>Header File Index</title>\n");
	fprintf(f, "<link rel=\"STYLESHEET\" type=\"TEXT/CSS\" href=\"style.css\">\n");
	fprintf(f, "</head>\n");
	fprintf(f, "<body bgcolor=\"#fffff8\">\n");
	fprintf(f, "<table class=\"INVTABLE\" width=\"100%\">\n");
	fprintf(f, "<tbody>\n");
	fprintf(f, "<tr>\n");
	fprintf(f, "<td class=\"NOBORDER\" width=\"40\"><img src=\"info.gif\" border=\"0\"\n");
	fprintf(f, "height=\"32\" width=\"32\"> </td>\n");
	fprintf(f, "<td class=\"TITLE\">%s<br>\n", topic_name);
	fprintf(f, "</td>\n");
	fprintf(f, "</tr>\n");
	fprintf(f, "</tbody>\n");
	fprintf(f, "</table>\n");

	return 0;
}

int main(int argc, char **argv)
{
	char *src_folder;
	char *doc_folder;
	char *txt_file;
	char *src_file;
	char *dst_file;
	FILE *f;
	FILE *fi, *fo;
	char line[65536];
	GList *l, *l2;

	// Check and get program arguments
#if 0
	if(argc < 3)
		return -1;

	doc_folder = argv[1];
	src_folder = argv[2];
#else
	// test
#ifdef __WIN32__
	src_folder = "C:\\sources\\roms\\tifiles-2\\src\\";
	doc_folder = "C:\\sources\\roms\\tifiles-2\\docs\\";
#else
	src_folder = "/home/devel/tilp_project/libs/files-2/src";
	doc_folder = "/home/devel/tilp_project/libs/files-2/docs";
#endif
#endif
	printf("Doc folder: <%s>\n", doc_folder);
	printf("Src folder: <%s>\n", src_folder);

	/* 
		Part 1: get list of topics 
	*/

	// Open list of topics ("api.txt")
	txt_file = g_strconcat(doc_folder, G_DIR_SEPARATOR_S, 
			       "tmpl", G_DIR_SEPARATOR_S, 
			       "api.txt", NULL);
	printf("Read list of topic from <%s>.\n", txt_file);
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
		str_array = g_strsplit(line, " : ", 2);

		t = g_malloc0(sizeof(topic));
		t->filename = str_array[0];
		t->basename = g_strdup("foobar");
		t->title = str_array[1];

		topics = g_list_append(topics, t);
	}

	// Close file
	fclose(f);

	/* 
		Part 2: get list of functions from topics 
	*/

	printf("Get lists of functions from topics.\n");
	for(l = topics; l != NULL; l = g_list_next(l))
	{
		topic *t = (topic *)l->data;
		gchar *path;
		
		path = g_strconcat(src_folder, G_DIR_SEPARATOR_S,
				   t->filename, NULL);
		get_list_of_functions(path, &t->fncts);
		g_free(path);
	}

	/* 
		Part 3: write API index 
	*/

	// Open "api.html" file
	src_file = g_strconcat(doc_folder, G_DIR_SEPARATOR_S, 
			       "tmpl", G_DIR_SEPARATOR_S, "api.html", 
			       NULL);
	fi = fopen(src_file, "rt");
	if(fi == NULL)
	{
		printf("Can't open input file: <%s>.\n", txt_file);
		return -1;
	}

	dst_file = g_strconcat(doc_folder, G_DIR_SEPARATOR_S, 
			       "html", G_DIR_SEPARATOR_S, "api.html", 
			       NULL);
	printf("Write API index in <%s>.\n", dst_file);
	fo = fopen(dst_file, "wt");
	if(fo == NULL)
	{
		printf("Can't open output file: <%s>.\n", dst_file);
		return -1;
	}

	// Insert list of topics and functions
	while(!feof(fi))
	{
		fgets(line, sizeof(line), fi);

		if(strstr(line, TOKEN1))
		{
			// parse topics and functions
			write_api_toc(fo);

			// skip </p>
			fgets(line, sizeof(line), fi);
		}

		fputs(line, fo);
	}

	// Close files
	fclose(fi);
	fclose(fo);

	/* 
		Part 4: write files by topics 
	*/

	printf("Write help per topics.\n");
	for(l = topics; l != NULL; l = g_list_next(l))
	{
		topic *t = (topic *)l->data;
		gchar *path;
		
		
	}

	/* 
		Part 5: release memory
	*/

	// Free memory
	for(l = topics; l != NULL; l = g_list_next(l))
        {
                topic *t = (topic *)l->data;

		g_free(t->filename);
		g_free(t->title);
		
		for(l2 = t->fncts; l2 != NULL; l2 = g_list_next(l2))
			g_free(l2->data);
		g_list_free(t->fncts);
        }
	g_list_free(topics);
	
	return 0;
}
