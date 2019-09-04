// File gathering group-related functionality, whose purpose is to be #include'd by main.cc.
// The authorship and license information are in main.cc.

// Check that a tigroup member name is safe to use as a plain file name: it
// must not be empty, "." or "..", and must not contain a directory separator.
static int tifileutil_member_name_unsafe(const char * name)
{
	return (!name || !name[0] || !strcmp(name, ".") || !strcmp(name, "..") || strchr(name, '/') != nullptr);
}

// Build "dirpath/name" as a malloc'ed string.
static char * tifileutil_make_path(const char * dirpath, const char * name)
{
	char * path = (char *)malloc(strlen(dirpath) + 1 + strlen(name) + 1);
	if (path) {
		sprintf(path, "%s/%s", dirpath, name);
	}
	return path;
}

// Check that a file can be put into a tigroup: single and group files carry
// the TIFILE_REGULAR bit, and flash files are accepted as well.
static int tifileutil_tigroupable(FileClass fc)
{
	return ((fc & TIFILE_REGULAR) || fc == TIFILE_FLASH);
}

// Check whether a member name is already present in the tigroup content.
static int tifileutil_group_member_present(TigContent * content, const char * name)
{
	unsigned int k;

	for (k = 0; k < content->n_vars; k++) {
		if (!strcmp(content->var_entries[k]->filename, name)) {
			return 1;
		}
	}
	for (k = 0; k < content->n_apps; k++) {
		if (!strcmp(content->app_entries[k]->filename, name)) {
			return 1;
		}
	}
	return 0;
}

// Merge the entries of the tigroup file @src into @content, rejecting
// duplicate member names and degenerate members; the entries are duplicated,
// so that the source tigroup content can be deleted without side effects.
// Return 0 on success, non-zero on error (message already printed).
static int tifileutil_group_merge_tigroup(char *** argv, TigContent * content, const char * src, const char * label)
{
	TigContent * tig = tifiles_content_create_tigroup(CALC_NONE, 0);
	unsigned int k;
	int ret;

	if (!tig) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		return 1;
	}
	ret = err_print(argv, "tifiles_file_read_tigroup", tifiles_file_read_tigroup(src, tig));
	if (ret) {
		tifiles_content_delete_tigroup(tig);
		return 1;
	}
	for (k = 0; k < tig->n_vars; k++) {
		TigEntry * te = tig->var_entries[k];
		TigEntry * nte;

		if (tifileutil_group_member_present(content, te->filename)) {
			tifileutil_critical(_("%s: duplicate entry %s in %s."), (*argv)[0], te->filename, label);
			tifiles_content_delete_tigroup(tig);
			return 1;
		}
		// Degenerate members (regular files with no variable entry, e.g. an
		// empty data section) are not valid tigroup members.
		if (!te->content.regular || !te->content.regular->num_entries) {
			tifileutil_critical(_("%s: %s is not a file which can be put into a tigroup."), (*argv)[0], te->filename);
			tifiles_content_delete_tigroup(tig);
			return 1;
		}
		nte = tifiles_te_create(te->filename, te->type, content->model);
		if (!nte) {
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			tifiles_content_delete_tigroup(tig);
			return 1;
		}
		nte->content.regular = tifiles_content_dup_regular(te->content.regular);
		tifiles_content_add_te(content, nte);
	}
	for (k = 0; k < tig->n_apps; k++) {
		TigEntry * te = tig->app_entries[k];
		TigEntry * nte;

		if (tifileutil_group_member_present(content, te->filename)) {
			tifileutil_critical(_("%s: duplicate entry %s in %s."), (*argv)[0], te->filename, label);
			tifiles_content_delete_tigroup(tig);
			return 1;
		}
		nte = tifiles_te_create(te->filename, te->type, content->model);
		if (!nte) {
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			tifiles_content_delete_tigroup(tig);
			return 1;
		}
		nte->content.flash = tifiles_content_dup_flash(te->content.flash);
		tifiles_content_add_te(content, nte);
	}
	tifiles_content_delete_tigroup(tig);
	return 0;
}

// Add the files listed in argv[first_file..argc-1] to @content: single, group
// and flash files become one entry each, tigroup files are merged entry by
// entry. Return 0 on success, non-zero on error (message already printed).
static int tifileutil_group_add_files(int * argc, char *** argv, unsigned int first_file, TigContent * content, const char * label)
{
	unsigned int i;

	for (i = first_file; i < (unsigned int)*argc; i++) {
		const char * src = (const char *)(*argv)[i];
		FileClass fc = tifiles_file_get_class(src);

		if (fc == TIFILE_TIGROUP) {
			if (tifileutil_group_merge_tigroup(argv, content, src, label)) {
				return 1;
			}
		}
		else if (tifileutil_tigroupable(fc)) {
			const char * base = strrchr(src, '/');
			CalcModel model = tifiles_file_get_model(src);
			TigEntry * te;
			int ret;

			base = base ? base + 1 : src;
			if (tifileutil_group_member_present(content, base)) {
				tifileutil_critical(_("%s: duplicate entry %s in %s."), (*argv)[0], base, label);
				return 1;
			}
			te = tifiles_te_create(src, fc, model);
			if (!te) {
				tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
				return 1;
			}
			ret = (fc == TIFILE_FLASH) ? err_print(argv, "tifiles_file_read_flash", tifiles_file_read_flash(src, te->content.flash))
			                           : err_print(argv, "tifiles_file_read_regular", tifiles_file_read_regular(src, te->content.regular));
			if (ret) {
				tifiles_te_delete(te);
				return 1;
			}
			// Reject regular files with no variable entry (e.g. an empty data
			// section), which would create a degenerate tigroup member.
			if (fc != TIFILE_FLASH && !te->content.regular->num_entries) {
				tifiles_te_delete(te);
				tifileutil_critical(_("%s: %s is not a file which can be put into a tigroup."), (*argv)[0], src);
				return 1;
			}
			tifiles_content_add_te(content, te);
		}
		else {
			tifileutil_critical(_("%s: %s is not a file which can be put into a tigroup."), (*argv)[0], src);
			return 1;
		}
	}
	return 0;
}

static int tifileutil_group_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s group -m MODE [OPTIONS] [FILE ...]
Where MODE may be:
 list:          list the entries of a tigroup file
 create:        create a new tigroup file from a list of files (single, group,
                flash or tigroup files; tigroups are merged entry by entry)
 extract:       extract the entries of a tigroup file into a directory
 append:        append a list of files to an existing tigroup file, or create
                a new one if the input file does not exist

All options must precede the file name(s).

OPTIONS for list mode:
 -v:            be verbose

OPTIONS for create mode:
 -t TYPE:       type of file to create, only "tigroup" is supported (required)
 -o OUTFILE:    output result to OUTFILE (required)
 -v:            be verbose

OPTIONS for extract mode:
 -d PATH:       extract into the PATH directory (default: current directory)
 -v:            be verbose

OPTIONS for append mode:
 -o OUTFILE:    output result to OUTFILE (default: the input tigroup file)
 -v:            be verbose
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_group_list(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * infilename = nullptr;
	unsigned int first_file = 0;
	int verbose = 0;
	TigContent * content = nullptr;
	unsigned int i;

	int j;

	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'm':
						// The group mode, already handled by the dispatcher.
						GET_OPTION_ARG_IGNORE("-m", tifileutil_group_usage);
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_group_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}
	if (!first_file) {
		return tifileutil_group_usage(argc, argv);
	}
	infilename = (const char *)(*argv)[first_file];

	if (!tifiles_file_is_tigroup(infilename)) {
		tifileutil_critical(_("%s: %s is not a tigroup file."), (*argv)[0], infilename);
		goto end;
	}
	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	if (!content) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	ret = err_print(argv, "tifiles_file_read_tigroup", tifiles_file_read_tigroup(infilename, content));
	if (ret) {
		ret = 3;
		goto end;
	}

	printf("Model: %s\n", tifiles_model_to_string(content->model));
	printf("Comment: %s\n", content->comment ? content->comment : "");
	printf("Entries: %u\n\n", content->n_vars + content->n_apps);
	printf(" %5s  %-8s  %s\n", "Size", "Class", "Name");
	printf(" %5s  %-8s  %s\n", "-----", "--------", "----");
	for (i = 0; i < content->n_vars; i++) {
		TigEntry * te = content->var_entries[i];
		// Degenerate members (e.g. a regular file with an empty data section)
		// parse successfully with no variable entry: print what we know.
		VarEntry * ve = (te->content.regular && te->content.regular->num_entries) ? te->content.regular->entries[0] : nullptr;
		if (ve) {
			const char * typestr = tifiles_vartype2string(content->model, ve->type);
			printf(" %5u  %-8s  %s (%s %s)\n", ve->size, "regular", te->filename, ve->name, typestr ? typestr : "");
		}
		else {
			printf(" %5u  %-8s  %s\n", 0U, "regular", te->filename);
		}
	}
	for (i = 0; i < content->n_apps; i++) {
		TigEntry * te = content->app_entries[i];
		printf(" %5u  %-8s  %s\n", te->content.flash->data_length, "flash", te->filename);
	}

	if (verbose) {
		tifileutil_info(_("%s: listed %u entries of %s."), (*argv)[0], content->n_vars + content->n_apps, infilename);
	}
	ret = 0;

end:
	if (content) {
		tifiles_content_delete_tigroup(content);
	}

	return ret;
}

static int tifileutil_group_create(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * typestr = nullptr;
	char * outfilename = nullptr;
	unsigned int first_file = 0;
	int verbose = 0;
	TigContent * content = nullptr;
	unsigned int i;

	int j;

	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'm':
						// The group mode, already handled by the dispatcher.
						GET_OPTION_ARG_IGNORE("-m", tifileutil_group_usage);
					break;

					case 't':
						GET_OPTION_ARG(typestr, "-t", tifileutil_group_usage);
					break;

					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_group_usage);
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_group_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}

	if (!typestr) {
		tifileutil_critical(_("%s: no group type specified."), (*argv)[0]);
		return tifileutil_group_usage(argc, argv);
	}
	if (strcmp(typestr, "tigroup")) {
		tifileutil_critical(_("%s: unsupported group type %s."), (*argv)[0], typestr);
		return tifileutil_group_usage(argc, argv);
	}
	if (!outfilename) {
		tifileutil_critical(_("%s: no output file specified."), (*argv)[0]);
		return tifileutil_group_usage(argc, argv);
	}
	if (!first_file) {
		return tifileutil_group_usage(argc, argv);
	}

	// Build the content from the input files (which can be single, group,
	// flash or tigroup files, the latter being merged entry by entry), and
	// write it only once every input has been validated.
	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	if (!content) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	if (tifileutil_group_add_files(argc, argv, first_file, content, outfilename)) {
		goto end;
	}
	ret = err_print(argv, "tifiles_file_write_tigroup", tifiles_file_write_tigroup(outfilename, content));
	if (ret) {
		ret = 3;
		goto end;
	}
	if (verbose) {
		tifileutil_info(_("%s: wrote %u entries to %s."), (*argv)[0], content->n_vars + content->n_apps, outfilename);
	}
	ret = 0;

end:
	if (content) {
		tifiles_content_delete_tigroup(content);
	}

	return ret;
}

static int tifileutil_group_extract(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * infilename = nullptr;
	const char * dirpath = ".";
	unsigned int first_file = 0;
	int verbose = 0;
	TigContent * content = nullptr;
	unsigned int i, count = 0;
	struct stat st = {};

	int j;

	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'm':
						// The group mode, already handled by the dispatcher.
						GET_OPTION_ARG_IGNORE("-m", tifileutil_group_usage);
					break;

					case 'd':
						GET_OPTION_ARG(dirpath, "-d", tifileutil_group_usage);
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_group_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}
	if (!first_file) {
		return tifileutil_group_usage(argc, argv);
	}
	infilename = (const char *)(*argv)[first_file];

	if (stat(dirpath, &st) < 0 || !S_ISDIR(st.st_mode)) {
		tifileutil_critical(_("%s: %s is not a directory."), (*argv)[0], dirpath);
		goto end;
	}
	if (!tifiles_file_is_tigroup(infilename)) {
		tifileutil_critical(_("%s: %s is not a tigroup file."), (*argv)[0], infilename);
		goto end;
	}
	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	if (!content) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	ret = err_print(argv, "tifiles_file_read_tigroup", tifiles_file_read_tigroup(infilename, content));
	if (ret) {
		ret = 3;
		goto end;
	}

	for (i = 0; i < content->n_vars; i++) {
		TigEntry * te = content->var_entries[i];
		char * outname;

		if (tifileutil_member_name_unsafe(te->filename)) {
			tifileutil_critical(_("%s: entry %s has an unsafe name."), (*argv)[0], te->filename);
			ret = 3;
			goto end;
		}
		outname = tifileutil_make_path(dirpath, te->filename);
		if (!outname) {
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		ret = err_print(argv, "tifiles_file_write_regular", tifiles_file_write_regular(outname, te->content.regular, 0));
		free(outname);
		if (ret) {
			ret = 3;
			goto end;
		}
		count++;
	}
	for (i = 0; i < content->n_apps; i++) {
		TigEntry * te = content->app_entries[i];
		char * outname;

		if (tifileutil_member_name_unsafe(te->filename)) {
			tifileutil_critical(_("%s: entry %s has an unsafe name."), (*argv)[0], te->filename);
			ret = 3;
			goto end;
		}
		outname = tifileutil_make_path(dirpath, te->filename);
		if (!outname) {
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		ret = err_print(argv, "tifiles_file_write_flash", tifiles_file_write_flash(outname, te->content.flash));
		free(outname);
		if (ret) {
			ret = 3;
			goto end;
		}
		count++;
	}

	if (verbose) {
		tifileutil_info(_("%s: extracted %u entries to %s."), (*argv)[0], count, dirpath);
	}
	ret = 0;

end:
	if (content) {
		tifiles_content_delete_tigroup(content);
	}

	return ret;
}

static int tifileutil_group_append(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * tigfilename = nullptr;
	char * outfilename = nullptr;
	unsigned int first_file = 0;
	int verbose = 0;
	TigContent * content = nullptr;
	unsigned int i;

	int j;

	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'm':
						// The group mode, already handled by the dispatcher.
						GET_OPTION_ARG_IGNORE("-m", tifileutil_group_usage);
					break;

					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_group_usage);
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_group_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}
	// The first file name is the tigroup, the remaining ones the files to append.
	if (!first_file || first_file + 1 >= (unsigned int)*argc) {
		return tifileutil_group_usage(argc, argv);
	}
	tigfilename = (const char *)(*argv)[first_file];

	content = tifiles_content_create_tigroup(CALC_NONE, 0);
	if (!content) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	if (tifiles_file_is_tigroup(tigfilename)) {
		ret = err_print(argv, "tifiles_file_read_tigroup", tifiles_file_read_tigroup(tigfilename, content));
		if (ret) {
			ret = 3;
			goto end;
		}
	}
	else {
		struct stat st = {};
		// A nonexistent input file is fine: behave like "create". An existing
		// file which is not a tigroup is an error, so as not to clobber it.
		if (stat(tigfilename, &st) == 0 || errno != ENOENT) {
			tifileutil_critical(_("%s: %s is not a tigroup file."), (*argv)[0], tigfilename);
			goto end;
		}
	}

	if (tifileutil_group_add_files(argc, argv, first_file + 1, content, outfilename ? outfilename : tigfilename)) {
		goto end;
	}

	// Write to -o OUTFILE if given, otherwise rewrite the input tigroup in place.
	ret = err_print(argv, "tifiles_file_write_tigroup", tifiles_file_write_tigroup(outfilename ? outfilename : tigfilename, content));
	if (ret) {
		ret = 3;
		goto end;
	}
	if (verbose) {
		tifileutil_info(_("%s: appended %u entries to %s."), (*argv)[0], content->n_vars + content->n_apps, outfilename ? outfilename : tigfilename);
	}
	ret = 0;

end:
	if (content) {
		tifiles_content_delete_tigroup(content);
	}

	return ret;
}

static int tifileutil_group(int * argc, char *** argv, unsigned int offset)
{
	const char * modestr = nullptr;

	int i, j;

	for (i = offset + 2; i < *argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'm':
						GET_OPTION_ARG(modestr, "-m", tifileutil_group_usage);
					break;

					default:
						// Other options are handled by the sub-modes.
					break;
				}
			}
		}
		else {
			// All options must precede the file names.
			break;
		}
	}

	if (!modestr) {
		return tifileutil_group_usage(argc, argv);
	}
	if (!strcmp(modestr, "list")) {
		return tifileutil_group_list(argc, argv, offset);
	}
	if (!strcmp(modestr, "create")) {
		return tifileutil_group_create(argc, argv, offset);
	}
	if (!strcmp(modestr, "extract")) {
		return tifileutil_group_extract(argc, argv, offset);
	}
	if (!strcmp(modestr, "append")) {
		return tifileutil_group_append(argc, argv, offset);
	}
	tifileutil_critical(_("%s: unknown group mode %s."), (*argv)[0], modestr);
	return tifileutil_group_usage(argc, argv);
}
