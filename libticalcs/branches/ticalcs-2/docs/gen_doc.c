#include <stdio.h>
#include <glib.h>
#include <string.h>

#define TOKEN1	"==========[ List of entries ]=========="

GList *topics = NULL;

// Used to describe a topic (read from api.txt)
typedef struct
{
	gchar*	filename;  // such as error.c
	gchar*  basename;  // such as error.c.html
	gchar*	title;     // such as "General Functions"
	GList*  fncts;     // a linked list of fnct structure
} topic;

// Used to describe a comment entry before a function
typedef struct
{
	gint   type;        // structure/enumeration (1) or function (0)
	gchar* title;       // such as " * tifiles_get_error:"
	gchar* declaration; // such as "TIEXPORT int TICALL tifiles_error_get("
	GList* args;        // such as " * @number: error number"
	gchar* comment;     // such as "Attempt to match the..."
	gchar* returns;     // such as "Return value: 0 if error"
} fnct;

// Used to describe a comment entry on an argument
typedef struct
{
	gchar *name;     // such as "number"
	gchar *comment; // such as "error number"
} arg;

// Open a C or H file and search for comment on function.
static int get_list_of_functions(const char *filename, GList **fncts)
{
	FILE *fi;
	char line[65536];

// open file	
	fi = fopen(filename, "rt");
	if(fi == NULL)
	{
		printf("Can't open this file: <%s>\n", filename);
		return -1;
	}

// process
	while(!feof(fi))
	{
		fgets(line, sizeof(line), fi);
		if(line[0] == '/' && line[1] == '*' && 
		   line[2] == '*' && line[3] != '*')
		{
			char *d;
			fnct *f;

			// get title
			fgets(line, sizeof(line), fi);
			d = strrchr(line, ':');
			if(d != NULL)
				*d = '\0';
			
			f = g_malloc0(sizeof(fnct));
			f->title = g_strdup(line+3);
			//printf("[%s]\n", s);

			// check for arguments
			do
			{
				arg *a;
				gchar **str_array;

				fgets(line, sizeof(line), fi);
				if(line[3] != '@')
					break;
				//printf("<%s>\n", line);
				
				a = g_malloc0(sizeof(arg));
				str_array = g_strsplit(line, ": ", 2);
				a->name = g_strdup(str_array[0]+4);
				a->comment = g_strdup(str_array[1]);
				g_strfreev(str_array);
				//printf("<%s %s>\n", a->name, a->comment);

				f->args = g_list_append(f->args, a);
			} while(!feof(fi));

			// get comment
			f->comment = g_strdup("");
			do
			{
				gchar *tmp;

				fgets(line, sizeof(line), fi);
		
				tmp = g_strconcat(f->comment, line+3, NULL);
				g_free(f->comment);
				f->comment = tmp;
				//printf("%s", line);
			} while(strcmp(line, " *") <= 0);
			//printf("[%s]\n", f->comment);

                        // get return value
			fgets(line, sizeof(line), fi);
			f->returns = g_strdup("");
			while(!feof(fi) && strncmp(line, " **/", 4))
			{
				gchar *tmp;
				
				fgets(line, sizeof(line), fi);
				if(strncmp(line, " **/", 4) > 0)
					break;

				tmp = g_strconcat(f->returns, line+3, NULL);
				g_free(f->returns);
				f->returns = tmp;
			};

			d = strchr(f->returns, ':');
			if(d != NULL)
				memmove(f->returns, d+2, strlen(d));

			// get function declaration
			fgets(line, sizeof(line), fi);
			f->declaration = g_strdup(line);
			//printf("[%s]", line);
			if(strstr(line, "tifiles_"))
				f->type = 0;
			else if(strstr(line, "enum"))
			{
				f->type = 1;
				
				while(!feof(fi) && line[0] != '}')
				{
					gchar *tmp;
					
					fgets(line, sizeof(line), fi);
					tmp = g_strconcat(f->declaration, 
							  line, NULL);
					g_free(f->declaration);
					f->declaration = tmp;
				};
			}

			*fncts = g_list_append(*fncts, f);
		}
	}

// close file
	fclose(fi);
	return 0;
}

static void write_api_toc(FILE *fo)
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
		{
			fnct *f = (fnct *)m->data;
			fprintf(fo, "<li><a href=\"%s#%s\">%s</a></li>\n", 
				t->basename, f->title, f->title);
		}
		fprintf(fo, "</ul>\n");
	}
}

static void write_topic_header(FILE *fo, const char *topic_name)
{
	fprintf(fo, "<html>\n");
	fprintf(fo, "<head>\n");
	fprintf(fo, "<title>Header File Index</title>\n");
	fprintf(fo, "<link rel=\"STYLESHEET\" type=\"TEXT/CSS\" href=\"style.css\">\n");
	fprintf(fo, "</head>\n");
	fprintf(fo, "<body bgcolor=\"#fffff8\">\n");
	fprintf(fo, "<table class=\"INVTABLE\" width=\"100%%\">\n");
	fprintf(fo, "<tbody>\n");
	fprintf(fo, "<tr>\n");
	fprintf(fo, "<td class=\"NOBORDER\" width=\"40\"><img src=\"info.gif\" border=\"0\"\n");
	fprintf(fo, "height=\"32\" width=\"32\"> </td>\n");
	fprintf(fo, "<td class=\"TITLE\">%s<br>\n", topic_name);
	fprintf(fo, "</td>\n");
	fprintf(fo, "</tr>\n");
	fprintf(fo, "</tbody>\n");
	fprintf(fo, "</table>\n");
	fprintf(fo, "<hr>\n");
}

static void write_topic_end(FILE *fo)
{
	fprintf(fo, "<h3><a href=\"file:index.html\">Return to the main index</a> </h3>");
	fprintf(fo, "<br>");
	fprintf(fo, "<br>");
	fprintf(fo, "<br>");
	fprintf(fo, "</body>\n");
	fprintf(fo, "</html>\n");
}

static void write_fncts_content(FILE *fo, GList *fncts)
{
	GList *l, *m;

	for(l = fncts; l != NULL; l = g_list_next(l))
        {
		fnct *f = l->data;

                // title
		fprintf(fo, "<h3><a name=\"%s\"></a>%s</h3>\n", 
			f->title, f->title);

		// declaration
		fprintf(fo, "<table style=\"width: 100%%; text-align: left;\" border=\"0\" cellpadding=\"2\" cellspacing=\"2\">\n");
		fprintf(fo, "<tbody>\n");
		fprintf(fo, "<tr>\n");
		fprintf(fo, "<td style=\"vertical-align: top;\">%s</td>\n", f->declaration);
		fprintf(fo, "</tr>\n");
		fprintf(fo, "</tbody>\n");
		fprintf(fo, "</table>\n");
		fprintf(fo, "<br>\n");

		// how function work
		fprintf(fo, "%s<br>\n", f->comment);
		fprintf(fo, "<br>\n");

		// table begin
		fprintf(fo, "<table style=\"width: 100%%; text-align: left;\" border=\"0\" cellpadding=\"2\" cellspacing=\"2\">\n");
		fprintf(fo, "<tbody>\n");

		// write table with arguments
		for(m = f->args; m != NULL; m = g_list_next(m))
		{
			arg *a = m->data;

			//printf("<%s %s>\n", a->name, a->comment);

			fprintf(fo, "<tr>\n");
			fprintf(fo, "<td style=\"vertical-align: top;\">%s :<br></td>\n", a->name);
			fprintf(fo, "<td style=\"vertical-align: top;\">%s<br></td>\n", a->comment);
			fprintf(fo, "</tr>\n");
		}

		if(f->type == 0)
		{
			fprintf(fo, "<tr>\n");
			fprintf(fo, "<td style=\"vertical-align: top;\">%s<br></td>\n", "Return value :");
			fprintf(fo, "<td style=\"vertical-align: top;\">%s<br></td>\n", f->returns);
			fprintf(fo, "</tr>\n");
		}

		// table end
		fprintf(fo, "</tbody>\n");
		fprintf(fo, "</table>\n");
		fprintf(fo, "<br>\n");
	}
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
	src_folder = "C:\\sources\\roms\\ticalcs2\\src\\";
	doc_folder = "C:\\sources\\roms\\ticalcs2\\docs\\";
#else
	src_folder = "/home/devel/tilp_project/libs/cables-2/src";
	doc_folder = "/home/devel/tilp_project/libs/cables-2/docs";
#endif
#endif
	printf("Doc folder: <%s>\n", doc_folder);
	printf("Src folder: <%s>\n", src_folder);

	/* 
		Part 1: get list of topics 
	*/

	// Open list of topics ("api.txt")
	txt_file = g_strconcat(doc_folder, G_DIR_SEPARATOR_S, 
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
		
		// fill structure
		t = g_malloc0(sizeof(topic));
		t->filename = str_array[0];
		t->basename = g_strconcat(str_array[0], ".html", NULL);
		t->title = str_array[1];

		// add to linked list
		topics = g_list_append(topics, t);
	}

	// Close file
	fclose(f);

	/* 
		Part 2: get list of functions and comments from topics 
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
		GList *fncts = t->fncts;
		gchar *filename;
		
		// Open an html file
		filename = g_strconcat(doc_folder, G_DIR_SEPARATOR_S,
				       "html", G_DIR_SEPARATOR_S, 
				       t->basename, NULL);
		f = fopen(filename, "wt");
		if(f == NULL)
		{
			printf("Can't open this file: <%s>.\n", filename);
			return -1;
		}

		// Write topic header
		write_topic_header(f, t->title);
		
		// Write function title and description
		write_fncts_content(f, fncts);

		// Write topic epilog
		write_topic_end(f);

		// Close file
		g_free(filename);
		fclose(f);
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
