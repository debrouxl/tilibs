// File gathering wrap/unwrap-related functionality, whose purpose is to be #include'd by main.cc.
// The authorship and license information are in main.cc.

static int type_name_is_tokenized(CalcModel model, uint8_t type_id)
{
	if (model == CALC_TI73) {
		return (type_id != TI73_PRGM && type_id != TI73_ASM && type_id != TI73_DIR && type_id != TI73_AVAR && type_id < 0x20);
	}

	if (model == CALC_TI82 || model == CALC_TI83 || model == CALC_TI83P || model == CALC_TI84P || model == CALC_TI84PC) {
		return (   type_id != TI82_PRGM && type_id != TI82_PPGM && type_id != TI83p_APPOBJ && type_id != TI83p_APPV
		        && type_id != TI83p_TEMPPROGOBJ && type_id != TI83p_GRP && type_id < 0x20);
	}

	// Other models and types.
	return 0;
}

static int type_has_length(CalcModel model, uint8_t type_id)
{
	if (!ticonv_model_is_tiz80(model) && !ticonv_model_is_tiez80(model)) {
		return 0;
	}

	if (model == CALC_TI85 || model == CALC_TI86) {
		return (type_id == TI85_EQU || type_id == TI85_STRNG || type_id == TI85_PICT || type_id == TI85_PRGM);
	}

	if (model == CALC_TI82 || model == CALC_TI83) {
		return (type_id == TI82_YVAR || type_id == TI83_STRNG || type_id == TI82_PRGM || type_id == TI82_PPGM || type_id == TI82_PIC);
	}

	if (model == CALC_TI73) {
		return (type_id == TI73_EQU || type_id == TI73_STR || type_id == TI73_PRGM || type_id == TI73_ASM || type_id == TI73_PIC || type_id == TI73_AVAR);
	}

	return (type_id == TI83p_EQU || type_id == TI83p_STR || type_id == TI83p_PRGM || type_id == TI83p_ASM || type_id == TI83p_PIC || type_id == TI83p_APPV);
}

static int type_is_ti68k_oth(CalcModel model, uint8_t type_id)
{
	if (model == CALC_TI89 || model == CALC_TI92P || model == CALC_V200 || model == CALC_TI89T || model == CALC_TI89T_USB) {
		return (type_id == TI89_OTH);
	}

	// Other models and types.
	return 0;
}

static int protect_type(CalcModel model, uint8_t type_id)
{
	return ((   tifiles_calc_is_ti8x(model)
	         && model != CALC_TI85
	         && model != CALC_TI86
	         && type_id == TI82_PRGM) ? type_id + 1
	                                  : type_id);
}

static int complexify_type(CalcModel model, uint8_t type_id)
{
	if (tifiles_calc_is_ti9x(model)) {
		return ((type_id == TI89_LIST || type_id == TI89_MAT) ? type_id + 1 : type_id);
	}

	if (model == CALC_TI85 || model == CALC_TI86) {
		return (  (type_id == TI85_REAL || type_id == TI85_VECTR
		        || type_id == TI85_LIST || type_id == TI85_MATRX
		        || type_id == TI85_CONS) ? type_id + 1
		                                 : type_id);
	}

	if (model == CALC_TI82 || model == CALC_TI73) {
		return type_id;
	}

	// TI-83 Premium CE / TI-84 Plus CE family specific types.
	if (model == CALC_TI83PCE_USB) {
		if (type_id == TI83pce_SIMPLEFRAC) {
			return TI83pce_CPLXSIMPLEFRAC;
		}
		if (type_id == TI83pce_RADICAL) {
			return TI83pce_CPLXRADICAL;
		}
		if (type_id == TI83pce_PI) {
			return TI83pce_CPLXPI;
		}
		if (type_id == TI83pce_PISIMPLEFRAC) {
			return TI83pce_CPLXPISIMPLEFRAC;
		}
	}

	// TI-83+/84+/CE family: real numbers and lists.
	return ((type_id == TI83_REAL || type_id == TI83_LIST) ? type_id + TI83_CPLX : type_id);
}

static int tifileutil_wrap_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s wrap [OPTIONS] [FILE | -]
Where OPTIONS may include:
 -o OUTFILE:    output result to OUTFILE
 -n NAME:       set on-calc variable name to NAME (TI-Z80, TI-eZ80, TI-68k)
 -f NAME:       set on-calc variable folder to NAME (TI-68k)
 -t TYPE:       set variable type to TYPE (e.g. 82p)
 -V VERSION:    set file version to VERSION (TI-Z80, TI-eZ80)
 -c COMMENT:    set file comment to COMMENT (strftime format string)
 --raw-comment=COMMENT: set the file comment literally (no strftime expansion)
 -x EXTENSION:  set 1-4 chars "extension" for "other" variable type (TI-68k)
 -p:            protect program (TI-Z80, TI-eZ80)
 -C:            number/list/matrix is complex (TI-Z80, TI-eZ80)
 -a:            mark file for sending to archive (TI-Z80, TI-eZ80, TI-68k)
 -l:            mark file locked (TI-68k)
 -P:            pack data as a TI-83PCE/TI-84+CE Python AppVar (with -t 8xv)
 -r:            raw mode (no length bytes)
 -v:            be verbose
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_wrap(int * argc, char *** argv, unsigned int offset)
{
	int ret;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	char * varname = nullptr;
	char * foldername = nullptr;
	char * vartype = nullptr;
	const char * comment = "Created by " PACKAGE " " PACKAGE_VERSION;
	const char * versionstr = nullptr;
	const char * extensionstr = nullptr;
	int protect = 0;
	int complexify = 0;
	int archive = 0;
	int lock = 0;
	int rawmode = 0;
	int packpython = 0;
	int verbose = 0;
	int rawcomment = 0;
	CalcModel model;
	uint8_t type_id;
	FILE * infile;
	FileContent * fc = nullptr;
	VarEntry * ve = nullptr;
	unsigned long dsize, dalloc = 1024;
	bool outfilename_is_allocated = false;
	bool varname_is_allocated = false;

	int i, j;
	unsigned int argstart;
	char * p;
	const char * cp;

	// In tipack compatibility mode, argv[0] is the command name; in the
	// regular "tifileutil wrap" invocation, argv[1] is the mode name.
	argstart = (!strcmp((*argv)[0], "tipack")) ? offset + 1 : offset + 2;
	for (i = argstart; i < (int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			// The long-form option sets the comment literally, without the
			// strftime expansion of -c; the last option given wins.
			if (!strncmp((*argv)[i], "--raw-comment", 13)) {
				rawcomment = 1;
				GET_LONG_OPTION_ARG(comment, "--raw-comment", tifileutil_wrap_usage);
				continue;
			}
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_wrap_usage);
					break;

					case 'n':
						GET_OPTION_ARG(varname, "-n", tifileutil_wrap_usage);
					break;

					case 'f':
						GET_OPTION_ARG(foldername, "-f", tifileutil_wrap_usage);
					break;

					case 't':
						GET_OPTION_ARG(vartype, "-t", tifileutil_wrap_usage);
					break;

					case 'c':
						rawcomment = 0;
						GET_OPTION_ARG(comment, "-c", tifileutil_wrap_usage);
					break;

					case 'V':
						GET_OPTION_ARG(versionstr, "-V", tifileutil_wrap_usage);
					break;

					case 'x':
						GET_OPTION_ARG(extensionstr, "-x", tifileutil_wrap_usage);
					break;

					case 'p':
						protect = 1;
					break;

					case 'C':
						complexify = 1;
					break;

					case 'P':
						packpython = 1;
					break;

					case 'a':
						archive = 1;
					break;

					case 'l':
						lock = 1;
					break;

					case 'r':
						rawmode = 1;
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_wrap_usage(argc, argv);
				}
			}
		}
		else {
			// The input file: like the original tipack, options and the file
			// may be interleaved, and the last file name given wins.
			infilename = (const char *)(*argv)[i];
		}
	}

	/* Set default outfilename or vartype */

	if (!outfilename && vartype) {
		if (infilename) {
			outfilename = (char *)malloc(strlen(infilename) + strlen(vartype) + 2);
			if (!outfilename) {
				goto oom;
			}
			outfilename_is_allocated = true;
			strcpy(outfilename, infilename);
			if ((p = strrchr(outfilename, '.'))) {
				*p = 0;
			}
			strcat(outfilename, ".");
			strcat(outfilename, vartype);
		}
		else {
			outfilename = (char *)malloc(strlen(vartype) + 3);
			if (!outfilename) {
				goto oom;
			}
			outfilename_is_allocated = true;
			sprintf(outfilename, "a.%s", vartype);
		}
	}
	else if (outfilename && !vartype) {
		if ((p = strrchr(outfilename, '.'))) {
			vartype = &(p[1]);
		}
	}

	if (!vartype) {
		tifileutil_critical(_("%s: no variable type specified."), (*argv)[0]);
		return 1;
	}

	p = (char *)malloc(strlen(vartype) + 3);
	if (!p) {
oom:
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		ret = 1;
		goto end;
	}
	sprintf(p, "a.%s", vartype);
	model = tifiles_file_get_model(p);
	free(p);

	if (!model) {
		tifileutil_critical(_("%s: invalid variable type %s."), (*argv)[0], vartype);
		ret = 1;
		goto end;
	}

	type_id = tifiles_fext2vartype(model, vartype);
	cp = tifiles_vartype2fext(model, type_id);
	if (!cp || !cp[0]) {
		tifileutil_critical(_("%s: invalid variable type %s."), (*argv)[0], vartype);
		ret = 1;
		goto end;
	}

	if (packpython) {
		if (protect || complexify || rawmode) {
			tifileutil_critical(_("%s: -P cannot be combined with -p, -C or -r."), (*argv)[0]);
			ret = 1;
			goto end;
		}
		if (!ticonv_model_is_tiez80(model)
		 && model != CALC_TI83P && model != CALC_TI84P && model != CALC_TI84P_USB) {
			tifileutil_critical(_("%s: -P requires a TI-eZ80 model."), (*argv)[0]);
			ret = 1;
			goto end;
		}
		// Python scripts are stored in APP VAR (0x15) variables.
		type_id = TI84p_APPV;
	}

	if (protect) {
		type_id = protect_type(model, type_id);
	}
	if (complexify) {
		type_id = complexify_type(model, type_id);
	}

	if (!varname) {
		p = strrchr(outfilename, '.');
		if (p) {
			*p = 0;
		}
		if (type_name_is_tokenized(model, type_id)) {
			varname = ticonv_varname_tokenize(model, outfilename, type_id);
			varname_is_allocated = true;
		}
		else {
			varname = (char *)ticonv_varname_strdup(outfilename);
			for (i = 0; outfilename[i]; i++) {
				if (outfilename[i] >= 'A' && outfilename[i] <= 'Z') {
					varname[i] = outfilename[i];
				}
				else if (outfilename[i] >= 'a' && outfilename[i] <= 'z') {
					varname[i] = outfilename[i] + 'A' - 'a';
				}
				else if (outfilename[i] >= '0' && outfilename[i] <= '9') {
					varname[i] = outfilename[i];
				}
				else {
					varname[i] = '[';
				}
			}
			varname[i] = 0;
			varname_is_allocated = true;
		}
		if (p) {
			*p = '.';
		}
	}

	if (infilename) {
		infile = fopen(infilename, "rb");
		if (!infile) {
			perror(infilename);
			ret = 2;
			goto end;
		}
	}
	else {
		infilename = "(standard input)";
		infile = stdin;
	}

	ve = tifiles_ve_create_alloc_data(dalloc);
	if (nullptr == ve) {
		goto oom;
	}
	if (!rawmode && !packpython && type_has_length(model, type_id)) {
		dsize = 2;
	}
	else {
		dsize = 0;
	}
	ve->size = dsize;

	i = fgetc(infile);
	while (!feof(infile) && !ferror(infile)) {
		if (dsize >= dalloc) {
			dalloc += 1024;
			ve->size = dsize;
			VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
			if (nullptr == ve2) {
				goto oom;
			}
		}
		ve->data[dsize++] = i;
		i = fgetc(infile);
	}
	ve->size = dsize;

	if (infile != stdin) {
		fclose(infile);
	}

	if (packpython) {
		// Build the legacy TI-eZ80 Python AppVar payload: [2-byte size][magic "PYCD"][0x00][script bytes].
		unsigned long payload_length, script_length;

		// Strip a UTF-8 BOM from the script, like TI-Connect is known to do.
		if (dsize >= 3 && ve->data[0] == 0xEF && ve->data[1] == 0xBB && ve->data[2] == 0xBF) {
			memmove(ve->data, ve->data + 3, dsize - 3);
			dsize -= 3;
		}
		script_length = dsize;
		payload_length = script_length + 4 + 1;
		if (dalloc < dsize + 7) {
			dalloc = dsize + 7;
			ve->size = dsize;
			VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
			if (nullptr == ve2) {
				goto oom;
			}
		}
		memmove(ve->data + 7, ve->data, script_length);
		ve->data[0] = payload_length & 0xff;
		ve->data[1] = (payload_length >> 8) & 0xff;
		ve->data[2] = 'P';
		ve->data[3] = 'Y';
		ve->data[4] = 'C';
		ve->data[5] = 'D';
		ve->data[6] = 0;
		dsize = 7 + script_length;
		ve->size = dsize;
	}
	else if (!rawmode && type_has_length(model, type_id)) {
		ve->data[0] = (dsize - 2) & 0xff;
		ve->data[1] = ((dsize - 2) >> 8) & 0xff;
	}

	if (nullptr != extensionstr && extensionstr[0] != 0 && type_is_ti68k_oth(model, type_id)) {
		if (dalloc < dsize + 7) {
			dalloc = dsize + 7;
			ve->size = dsize;
			VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
			if (nullptr == ve2) {
				goto oom;
			}
		}
		ve->data[dsize++] = 0;
		ve->data[dsize++] = extensionstr[0];
		if (extensionstr[1] != 0) {
			ve->data[dsize++] = extensionstr[1];
			if (extensionstr[2] != 0) {
				ve->data[dsize++] = extensionstr[2];
				if (extensionstr[3] != 0) {
					ve->data[dsize++] = extensionstr[3];
				}
			}
		}
		ve->data[dsize++] = 0;
		ve->data[dsize++] = 0xF8; // OTH_TAG
		ve->size = dsize;
	}

	// TI-68k files carry:
	// * a 2-byte size field containing the length of the subsequent data;
	// * a trailing tag which indicates the variable's type.
	// Build the structure so that the wrapped file is well-formed and can round-trip
	// through deep unwrapping; attempt to preserve compatibility with strings
	// containing 0x00 bytes, even though newer versions of TI-Connect truncate
	// contents in that case.
	if (!rawmode && ticonv_model_is_ti68k(model)) {
		if (type_id == TI89_ASM) {
			// Structure: [2-byte size][N-byte code][N-byte relocation data][0xF3].
			if (dalloc < dsize + 3) {
				dalloc = dsize + 3;
				ve->size = dsize;
				VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
				if (nullptr == ve2) {
					goto oom;
				}
			}
			memmove(ve->data + 2, ve->data, dsize);
			ve->data[0] = ((dsize + 1) >> 8) & 0xff;
			ve->data[1] = (dsize + 1) & 0xff;
			ve->data[dsize + 2] = 0xF3; // ASM_TAG
			dsize += 3;
			ve->size = dsize;
		}
		else if (type_id == TI89_STR) {
			// Structure: [2-byte size][0x00][string bytes][0x00][0x2D].
			if (dalloc < dsize + 5) {
				dalloc = dsize + 5;
				ve->size = dsize;
				VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
				if (nullptr == ve2) {
					goto oom;
				}
			}
			memmove(ve->data + 3, ve->data, dsize);
			ve->data[0] = ((dsize + 3) >> 8) & 0xff;
			ve->data[1] = (dsize + 3) & 0xff;
			ve->data[2] = 0;
			ve->data[dsize + 3] = 0;
			ve->data[dsize + 4] = 0x2D; // STR_TAG
			dsize += 5;
			ve->size = dsize;
		}
		else if (type_id == TI89_OTH && extensionstr && extensionstr[0] != 0) {
			// Structure: [2-byte size][data bytes][0x00][extension][0x00][0xF8],
			// the footer having been appended above.
			if (dalloc < dsize + 2) {
				dalloc = dsize + 2;
				ve->size = dsize;
				VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
				if (nullptr == ve2) {
					goto oom;
				}
			}
			memmove(ve->data + 2, ve->data, dsize);
			ve->data[0] = (dsize >> 8) & 0xff;
			ve->data[1] = dsize & 0xff;
			dsize += 2;
			ve->size = dsize;
		}
		else if (type_id == TI89_TEXT) {
			// Structure: [2-byte size][2-byte cursor offset][lines...][0xE0],
			// where each line is [1-byte line type][characters][0x0D line break or 0x00 EOF]
			// * the input is conveniently assumed to be in the appropriate character set;
			// * the cursor position is set to the beginning of file, at offset 1;
			// * the line type byte is hard-coded to 0x20, ignoring the possibility of commands;
			// * CR, LF and CR+LF EOLs are mapped to CR.
			unsigned long outsize = 3 * dsize + 8, outpos = 2, i2 = 0;
			uint8_t * newdata;

			newdata = (uint8_t *)tifiles_ve_alloc_data(outsize);
			if (!newdata) {
				goto oom;
			}
			newdata[outpos++] = 0x00;
			newdata[outpos++] = 0x01;
			while (i2 < dsize) {
				unsigned long linestart = i2;
				newdata[outpos++] = 0x20; // Normal line type.
				while (i2 < dsize && ve->data[i2] != '\r' && ve->data[i2] != '\n') {
					i2++;
				}
				memcpy(newdata + outpos, ve->data + linestart, i2 - linestart);
				outpos += i2 - linestart;
				if (i2 < dsize) {
					// Consume the line ending, be it CR, LF or CR+LF.
					if (ve->data[i2] == '\r' && i2 + 1 < dsize && ve->data[i2 + 1] == '\n') {
						i2 += 2;
					}
					else {
						i2++;
					}
					// Preserve the line break, including at EOF (a text whose last
					// line is CR-terminated must round-trip through deep unwrapping).
					newdata[outpos++] = 0x0D;
				}
			}
			if (outpos > 2+2) {
				newdata[outpos++] = 0x00; // End of file delimiter.
			}
			newdata[outpos++] = 0xE0; // TEXT_TAG.
			// Now fill in the 2-byte size field.
			newdata[0] = ((outpos - 2) >> 8) & 0xff;
			newdata[1] = (outpos - 2) & 0xff;
			tifiles_ve_free_data(ve->data);
			ve->data = newdata;
			dsize = outpos;
			ve->size = dsize;
		}
		else if (type_id == TI89_PIC) {
			// Structure: [2-byte size][2-byte height][2-byte width][1 bpp bitmap][0xDF].
			// The number of bytes is ceil(width/8)*height, with an upper bound of
			// (240/8)*103 = 3090 bytes corresponding to the 92+/V200 graph screen size.
			// The last row is padded.
			unsigned long stride, height, width, padding;
			unsigned long best_w = 1, best_h = 1, best_pad = ULONG_MAX;
			unsigned long w;

			if (dsize == 0 || dsize > 3090) {
				tifileutil_critical(_("%s: input data too small or too large for a TI-68k picture (max 3090 bytes)."), (*argv)[0]);
				ret = 3;
				goto end;
			}
			// Simple algorithm which attempts to minimize the amount of padding.
			for (w = 1; w <= 240; w++) {
				stride = (w + 7) / 8;
				height = (dsize + stride - 1) / stride;
				if (height >= 1 && height <= 103) {
					padding = height * stride - dsize;
					if (padding < best_pad || (padding == best_pad && w > best_w)) {
						best_pad = padding;
						best_w = w;
						best_h = height;
					}
				}
			}
			width = best_w;
			height = best_h;
			if (dalloc < 7 + dsize + best_pad) {
				dalloc = 7 + dsize + best_pad;
				ve->size = dsize;
				VarEntry * ve2 = tifiles_ve_realloc_data(ve, dalloc);
				if (nullptr == ve2) {
					goto oom;
				}
			}
			memmove(ve->data + 6, ve->data, dsize);
			ve->data[0] = ((5 + dsize + best_pad) >> 8) & 0xff;
			ve->data[1] = (5 + dsize + best_pad) & 0xff;
			ve->data[2] = (height >> 8) & 0xff;
			ve->data[3] = height & 0xff;
			ve->data[4] = (width >> 8) & 0xff;
			ve->data[5] = width & 0xff;
			memset(ve->data + 6 + dsize, 0, best_pad); // Padding of the last row.
			ve->data[6 + dsize + best_pad] = 0xDF; // PIC_TAG.
			dsize += 7 + best_pad;
			ve->size = dsize;
		}
	}

	fc = tifiles_content_create_regular(model);

	if (comment) {
		tifileutil_comment_set(fc->comment, 40, comment, rawcomment);
	}

	memset(ve->folder, 0, FLDNAME_MAX);
	if (nullptr != foldername) {
		strncpy(ve->folder, foldername, VARNAME_MAX);
	}
	memset(ve->name, 0, VARNAME_MAX);
	strncpy(ve->name, varname, VARNAME_MAX);
	ve->type = type_id;
	ve->attr = (archive ? ATTRB_ARCHIVED : 0) | (lock ? ATTRB_LOCKED : 0);
	// This usage of atoi()'s lack of error checking is intentional.
	// It represents an easy and graceful fallback to 0 if the version string isn't an integer.
	ve->version = (versionstr == nullptr ? 0 : atoi(versionstr));
	ve->size = dsize;

	// tifiles_content_add_entry returns the number of entries.
	i = !tifiles_content_add_entry(fc, ve);
	if (!i) {
		i = err_print(argv, "tifiles_file_write_regular", tifiles_file_write_regular(outfilename, fc, 0));

		if (!i && verbose) {
			tifiles_file_display_regular(fc);
		}

		/*i = */err_print(argv, "tifiles_content_del_entry", tifiles_content_del_entry(fc, ve));
		ve = nullptr;
	}

	ret = i ? 3 : 0;

end:
	if (ve) {
		tifiles_ve_delete(ve);
	}
	tifiles_content_delete_regular(fc);

	if (varname_is_allocated) {
		ticonv_varname_free(varname);
	}
	if (outfilename_is_allocated) {
		free(outfilename);
	}

	return ret;
}

static int unwrap_deep_python(char *** argv, VarEntry * ve, uint8_t ** out, uint32_t * outlen)
{
	uint32_t pos, script_offset = 0;
	uint16_t size;
	uint32_t length;
	unsigned int shift, length_byte_count;
	uint8_t byte;

	*out = nullptr;
	*outlen = 0;

	// Legacy CE Python AppVar payload: [2-byte size][magic "PYCD"/"PYSC"][metadata records][0x00][script bytes].
	// Return values: 0 on success, 1 on allocation failure, 2 if the data is not a Python AppVar
	// payload (caller passes it through), 3 on malformed payload.
	if (ve->size < 2 + 4 + 1) {
		return 2;
	}
	size = ve->data[0] | ((uint16_t)ve->data[1] << 8);
	if (size != ve->size - 2) {
		tifileutil_warning(_("%s: Python AppVar size field (%u) differs from actual data length (%u)."), (*argv)[0], size, ve->size - 2);
	}
	if (memcmp(ve->data + 2, "PYCD", 4) && memcmp(ve->data + 2, "PYSC", 4)) {
		return 2;
	}

	// Metadata records: [LEB128 length, value = type + record data byte count][type byte][record data...],
	// terminated by a 0x00 byte; the script follows the terminator.
	pos = 2 + 4;
	while (pos < ve->size && ve->data[pos] != 0x00) {
		length = 0;
		shift = 0;
		length_byte_count = 0;
		while (pos < ve->size) {
			byte = ve->data[pos++];
			length |= (uint32_t)(byte & 0x7F) << shift;
			length_byte_count++;
			if (!(byte & 0x80)) {
				break;
			}
			shift += 7;
			if (length_byte_count >= 2) {
				return 3; // overlong metadata record length
			}
		}
		if (length_byte_count == 0 || length == 0 || pos + length > ve->size) {
			return 3; // malformed metadata record
		}
		pos += length; // Skip the type byte and the record data.
	}
	if (pos >= ve->size) {
		return 3; // metadata terminator not found
	}
	script_offset = pos + 1;

	*out = (uint8_t *)malloc(ve->size - script_offset);
	if (!*out) {
		return 1;
	}
	memcpy(*out, ve->data + script_offset, ve->size - script_offset);
	*outlen = ve->size - script_offset;
	return 0;
}

static int unwrap_deep_entry(char *** argv, CalcModel model, VarEntry * ve, uint8_t ** out, uint32_t * outlen)
{
	uint32_t start = 0, end = ve->size;
	uint8_t * tokenized_name = nullptr;

	// TI-83+/84+/CSE/CE PPRGM: strip the 2-byte length field, if present,
	// as well as the tokenized name and its 0x00 terminator.
	if (ve->type == TI83p_ASM) {
		bool is_83P_class = (model == CALC_TI83P || model == CALC_TI84P || model == CALC_TI84P_USB || model == CALC_TI82A_USB || model == CALC_TI84PT_USB);
		bool is_84PCSE_class = (model == CALC_TI84PC || model == CALC_TI84PC_USB);
		bool is_eZ80_class = ticonv_model_is_tiez80(model);
		if (is_83P_class || is_84PCSE_class || is_eZ80_class) {
			// The program data starts with a 2-byte length field, the length of the rest;
			// skip it if present.
			if ((ve->size >= 2 && (uint32_t)(ve->data[0] | ((uint16_t)ve->data[1] << 8)) == ve->size - 2)
			 || (ve->size >= 2 && (uint32_t)(ve->data[0] | ((uint16_t)ve->data[1] << 8)) == ve->size)) {
				start = 2;
			}
			// Squished programs start with the model-specific marker and contain no name prefix.
			if ((ve->size >= start + 2) &&    ((ve->data[start] == 0xBB && ve->data[start + 1] == 0x6D && is_83P_class)
			                               ||  (ve->data[start] == 0xEF && ve->data[start + 1] == 0x69 && is_84PCSE_class)
			                               ||  (ve->data[start] == 0xEF && ve->data[start + 1] == 0x7B && is_eZ80_class))) {
				tifileutil_info(_("%s: squished program, no name prefix to strip."), (*argv)[0]);
			}
			else {
				tokenized_name = (uint8_t *)ticonv_varname_tokenize(model, ve->name, ve->type);
				if (   tokenized_name && tokenized_name[0] && ve->size > start + (uint32_t)strlen((char *)tokenized_name)
				    && !memcmp(ve->data + start, tokenized_name, strlen((char *)tokenized_name))
				    && ve->data[start + strlen((char *)tokenized_name)] == 0x00) {
					start += strlen((char *)tokenized_name) + 1;
				}
				else {
					tifileutil_warning(_("%s: no tokenized name prefix found, passing data through."), (*argv)[0]);
				}
			}
		}
	}
	else if (ticonv_model_is_ti68k(model)) {
		if (ve->type == TI89_ASM) {
			// ASM programs: [2-byte size field][N-byte code][N-byte relocation data][0xF3]
			// Strip the size field and the tag; keep the relocation data and code.
			if (start + 2 <= ve->size) {
				start += 2;
			}
			if (end > start && ve->data[end - 1] == 0xF3) {
				end--;
			}
			else if (end > start) {
				tifileutil_warning(_("%s: no 0xF3 tag found, passing the last byte through."), (*argv)[0]);
			}
		}
		else if (ve->type == TI89_STR) {
			// Structure: [2-byte size][0x00][string bytes][0x00][0x2D].
			// Strip the envelope bytes and the tag; keep the string contents, including 0x00 bytes,
			// a practice no longer supported by e.g. modern versions of TI-Connect.
			if (start + 2 <= ve->size) {
				uint16_t strsize = ((uint16_t)ve->data[start] << 8) | ve->data[start + 1];
				start += 2;
				if (strsize != ve->size - start) {
					tifileutil_warning(_("%s: string size field (%u) differs from actual data length (%u)."), (*argv)[0], strsize, ve->size - start);
				}
			}
			if (end > start && ve->data[start] == 0x00) {
				start++;
			}
			else if (end > start) {
				tifileutil_warning(_("%s: no leading 0x00 byte found, passing data through."), (*argv)[0]);
			}
			if (end > start + 1 && ve->data[end - 2] == 0x00 && ve->data[end - 1] == 0x2D) {
				end -= 2;
			}
			else if (end > start) {
				tifileutil_warning(_("%s: no STR_TAG (0x2D) found, passing the trailing bytes through."), (*argv)[0]);
			}
		}
		else if (ve->type == TI89_OTH) {
			// Structure: [2-byte size][data bytes][0x00][extension][0x00][0xF8],
			// Strip the header, the extension and the tag; keep the data.
			unsigned int extlen;
			uint32_t separator = 0;
			if (start + 2 <= ve->size) {
				uint16_t strsize = ((uint16_t)ve->data[start] << 8) | ve->data[start + 1];
				start += 2;
				if (strsize != ve->size - start) {
					tifileutil_warning(_("%s: other data size field (%u) differs from actual data length (%u)."), (*argv)[0], strsize, ve->size - start);
				}
			}
			for (extlen = 1; extlen <= 4 && separator == 0; extlen++) {
				if (end >= start + 3 + extlen && ve->data[end - 1] == 0xF8 && ve->data[end - 2] == 0x00 && ve->data[end - 3 - extlen] == 0x00) {
					separator = end - 3 - extlen;
				}
			}
			if (separator > start) {
				end = separator;
			}
			else if (end > start) {
				tifileutil_warning(_("%s: no OTH_TAG (0xF8) found, passing the trailing bytes through."), (*argv)[0]);
			}
		}
		else if (ve->type == TI89_TEXT) {
			// See tifileutil_wrap() for detailed format information.
			// Strip the header and tag; on every line, strip the command character.
			// Keep CR EOLs, don't perform character set conversion.
			uint32_t pos, linestart, outlen2 = 0;
			uint8_t * outdata;

			if (start + 2 <= ve->size) {
				uint16_t strsize = ((uint16_t)ve->data[start] << 8) | ve->data[start + 1];
				start += 2;
				if (strsize != ve->size - start) {
					tifileutil_warning(_("%s: text size field (%u) differs from actual data length (%u)."), (*argv)[0], strsize, ve->size - start);
				}
			}
			if (start + 2 <= ve->size) {
				start += 2; // Skip cursor position.
			}
			if (end > start && ve->data[end - 1] == 0xE0) {
				end--;
			}
			else if (end > start) {
				tifileutil_warning(_("%s: no TEXT_TAG (0xE0) found, passing the last byte through."), (*argv)[0]);
			}

			outdata = (uint8_t *)malloc(end - start);
			if (!outdata) {
				return 1;
			}
			pos = start;
			while (pos < end) {
				uint8_t linetype = ve->data[pos++];
				linestart = pos;
				while (pos < end && ve->data[pos] != 0x0D && ve->data[pos] != 0x00) {
					pos++;
				}
				if (linetype != 0x0C && linetype != 0x20 && linetype != 0x43 && linetype != 0x50) {
					tifileutil_warning(_("%s: unknown text line type 0x%02X."), (*argv)[0], linetype);
				}
				for (uint32_t i = linestart; i < pos; i++) {
					outdata[outlen2++] = ve->data[i];
				}
				if (pos < end && ve->data[pos] == 0x0D) {
					outdata[outlen2++] = '\r'; // Line break.
					pos++;
				}
				else if (pos < end && ve->data[pos] == 0x00) {
					pos++; // End of file.
					break;
				}
			}
			if (pos != end) {
				tifileutil_warning(_("%s: trailing bytes after the end-of-file delimiter."), (*argv)[0]);
			}
			*out = outdata;
			*outlen = outlen2;
			return 0;
		}
		else if (ve->type == TI89_PIC) {
			// Structure: [2-byte size][2-byte height][2-byte width][1 bpp bitmap][0xDF].
			// Strip the header and tag; keep the bitmap data.
			uint16_t height = 0, width = 0;
			uint32_t bitmap_length;

			if (start + 2 <= ve->size) {
				uint16_t strsize = ((uint16_t)ve->data[start] << 8) | ve->data[start + 1];
				start += 2;
				if (strsize != ve->size - start) {
					tifileutil_warning(_("%s: picture size field (%u) differs from actual data length (%u)."), (*argv)[0], strsize, ve->size - start);
				}
			}
			if (start + 4 <= ve->size) {
				height = ((uint16_t)ve->data[start] << 8) | ve->data[start + 1];
				width = ((uint16_t)ve->data[start + 2] << 8) | ve->data[start + 3];
				start += 4;
			}
			bitmap_length = (uint32_t)height * ((width + 7) / 8);
			if (end > start && ve->data[end - 1] == 0xDF) {
				end--;
			}
			else if (end > start) {
				tifileutil_warning(_("%s: no PIC_TAG (0xDF) found, passing the last byte through."), (*argv)[0]);
			}
			if (bitmap_length != end - start) {
				tifileutil_warning(_("%s: bitmap length (%u) differs from height*ceil(width/8) (%u)."), (*argv)[0], end - start, bitmap_length);
			}
		}
	}
	// CE Python AppVars are detected by their payload magic, as regular files
	// don't reliably identify their model (the signature doesn't distinguish
	// TI-83+ from TI-eZ80, and the extension may or may not match the signature).
	else if (ve->type == TI83p_APPV) {
		int ret2 = unwrap_deep_python(argv, ve, out, outlen);
		if (ret2 == 2) {
			// Not a Python AppVar payload: pass the data through.
			tifileutil_warning(_("%s: not a Python AppVar payload, passing data through."), (*argv)[0]);
			*out = (uint8_t *)malloc(ve->size);
			if (!*out) {
				return 1;
			}
			memcpy(*out, ve->data, ve->size);
			*outlen = ve->size;
			return 0;
		}
		if (ret2 == 3) {
			tifileutil_critical(_("%s: malformed Python AppVar payload."), (*argv)[0]);
			return 1;
		}
		return ret2;
	}

	if (tokenized_name) {
		ticonv_varname_free((char *)tokenized_name);
	}

	if (end <= start) {
		*out = nullptr;
		*outlen = 0;
		return 0;
	}

	*out = (uint8_t *)malloc(end - start);
	if (!*out) {
		return 1;
	}
	memcpy(*out, ve->data + start, end - start);
	*outlen = end - start;
	return 0;
}

static int tifileutil_unwrap_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s unwrap [OPTIONS] FILE
Where OPTIONS may include:
 -o OUTFILE:    output result to OUTFILE (- for stdout; default stdout)
 -d:            deep unwrapping (beyond computer-side header and footer)
 -v:            be verbose
Note: all options must precede the file name(s).
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_unwrap(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	FILE * infile = nullptr;
	FILE * outfile = nullptr;
	int deep = 0;
	int verbose = 0;
	CalcModel model;
	FileContent * fc = nullptr;
	uint8_t * buffer = nullptr;
	unsigned long file_size = 0, lenread = 0, written = 0;
	unsigned int i;
	unsigned int first_file = 0;

	int j;

	// Skip argv[1], the command name.
	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_unwrap_usage);
					break;

					case 'd':
						deep = 1;
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_unwrap_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}

	if (!first_file) {
		return tifileutil_unwrap_usage(argc, argv);
	}
	infilename = (const char *)(*argv)[first_file];

	infile = fopen(infilename, "rb");
	if (!infile) {
		perror(infilename);
		ret = 2;
		goto end;
	}

	// Read the whole file, so that both pass-through and slicing modes can be handled.
	{
		long cur_pos;
		if (fseek(infile, 0, SEEK_END) < 0 || (cur_pos = ftell(infile)) < 0 || fseek(infile, 0, SEEK_SET) < 0) {
			tifileutil_critical(_("%s: cannot seek in input file."), (*argv)[0]);
			goto end;
		}
		file_size = cur_pos;
	}
	// The TI-Z80, TI-eZ80 and TI-68k series' members have at best 4 MB of Flash.
	if (file_size >= (4L << 20)) {
		tifileutil_critical(_("%s: input file too large."), (*argv)[0]);
		goto end;
	}
	buffer = (uint8_t *)malloc(file_size);
	if (!buffer) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	lenread = fread(buffer, 1, file_size, infile);
	if (lenread != file_size) {
		tifileutil_critical(_("%s: unable to read the entire input file."), (*argv)[0]);
		goto end;
	}

	if (outfilename && !(outfilename[0] == '-' && outfilename[1] == 0)) {
		outfile = fopen(outfilename, "wb");
		if (!outfile) {
			perror(outfilename);
			ret = 2;
			goto end;
		}
	}
	else {
		outfile = stdout;
	}

	// Flash files have no computer-side header/footer to strip: pass them through unchanged.
	if (tifiles_file_is_os(infilename) || tifiles_file_is_app(infilename)) {
		if (fwrite(buffer, 1, file_size, outfile) != file_size) {
			tifileutil_critical(_("%s: unable to write the entire output."), (*argv)[0]);
			goto end;
		}
		written = file_size;
		ret = 0;
		goto end;
	}

	if (!tifiles_file_is_regular(infilename)) {
		tifileutil_critical(_("%s: %s is not a regular TI file."), (*argv)[0], infilename);
		ret = 1;
		goto end;
	}

	model = tifiles_file_get_model(infilename);
	if (!model) {
		tifileutil_critical(_("%s: invalid input file %s."), (*argv)[0], infilename);
		goto end;
	}

	// Parse the file, both for validation and for deep unwrapping.
	fc = tifiles_content_create_regular(model);
	if (!fc) {
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	ret = err_print(argv, "tifiles_file_read_regular", tifiles_file_read_regular(infilename, fc));
	if (ret) {
		ret = 3;
		fc = nullptr; // tifiles_file_read_regular() frees the content on error.
		goto end;
	}
	if (fc->num_entries != 1) {
		tifileutil_warning(_("%s: multi-entry file, unwrapping assumes a single entry."), (*argv)[0]);
	}

	if (deep) {
		// Deep unwrapping: transform each entry's data and write the raw bytes.
		for (i = 0; i < fc->num_entries; i++) {
			uint8_t * entry_data = nullptr;
			uint32_t entry_length = 0;

			if (unwrap_deep_entry(argv, model, fc->entries[i], &entry_data, &entry_length)) {
				tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
				free(entry_data);
				ret = 3;
				goto end;
			}
			if (entry_length && fwrite(entry_data, 1, entry_length, outfile) != entry_length) {
				tifileutil_critical(_("%s: unable to write the entire output."), (*argv)[0]);
				free(entry_data);
				goto end;
			}
			written += entry_length;
			free(entry_data);
		}
		ret = 0;
	}
	else {
		// Simple unwrapping: strip the header and the trailing checksum.
		// Use the previously parsed data structures.
		if (fwrite(fc->entries[0]->data, 1, fc->entries[0]->size, outfile) != fc->entries[0]->size) {
			tifileutil_critical(_("%s: unable to write the entire output."), (*argv)[0]);
			goto end;
		}
		written = fc->entries[0]->size;
		ret = 0;
	}

	if (verbose) {
		tifileutil_info(_("%s: wrote %lu bytes."), (*argv)[0], written);
	}

end:
	if (infile && infile != stdin) {
		fclose(infile);
	}
	if (outfile && outfile != stdout) {
		fclose(outfile);
	}
	free(buffer);
	if (fc) {
		tifiles_content_delete_regular(fc);
	}

	return ret;
}
