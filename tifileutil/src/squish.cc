// File gathering squish/unsquish-related functionality, whose purpose is to be #include'd by main.cc.
// The authorship and license information are in main.cc.

static int data_has_prefix(const uint8_t * data, uint32_t length, const uint8_t * prefix, uint32_t prefix_length)
{
	return (length >= prefix_length && !memcmp(data, prefix, prefix_length));
}

enum { SHELL_NONE,
       SHELL_82_CRASH, SHELL_82_ACE, SHELL_82_ASH, SHELL_82_SNG,
       SHELL_73_MALLARD, SHELL_73_NOSTUB,
       SHELL_83_ION, SHELL_82_ASHELL, SHELL_83_VENUS, SHELL_83_ION83, SHELL_83_PSE83, SHELL_83_ICE, SHELL_83_SOS,
       SHELL_83P_TSE, SHELL_83P_MIRAGE };

// Squished (executable) ASM programs start with a model-specific marker:
// TI-82 CrASH: D5 00 11; TI-83+/84+: BB 6D (the Asm( prefix); TI-84+CSE: EF 69;
// TI-83PCE/TI-84PCE: EF 7B. TI-73/83 have no marker.
static const uint8_t squished_prefix_82_crash[] = { 0xD5, 0x00, 0x11 };      // CrASH
static const uint8_t squished_prefix_82_ace[] = { 0xD5, 0x00, 0xA0 };  // ACE
static const uint8_t squished_prefix_82_ash[] = { 0xD9, 0x00, 0x30 };  // ASH
static const uint8_t squished_prefix_82_sng[] = { 'S', 'N', 'G' };     // SNG
static const uint8_t squished_prefix_83p[] = { 0xBB, 0x6D };
static const uint8_t squished_prefix_cse[] = { 0xEF, 0x69 };
static const uint8_t squished_prefix_ce[] = { 0xEF, 0x7B };
// Unsquished TI-73/83 programs end with the tokenized "End 0000 End" trailer.
static const uint8_t squish_trailer[] = { 0x3F, 0xD4, 0x3F, 0x30, 0x30, 0x30, 0x30, 0x3F, 0xD4 };

// TI-83 Venus programs start with the tokenized BASIC "Send(9prgm0V" magic,
// "ç9_[V?" + NUL, optionally followed by a "jr nc" skipping the description.
static const uint8_t squished_venus_magic[] = { 0xE7, 0x39, 0x5F, 0x5B, 0x56, 0x3F, 0x00 };

// TI-73 Mallard programs start with the D9 00 magic, a fixed 4-character
// program name ("Duck" in every program built with the official template), a
// 2-byte entry point (the address of the code start, at userMem + header
// length, i.e. 0x9C37 + 2 + 4 + 2 + strlen(description) + 1 on the TI-73),
// and a NUL-terminated description.
static const uint8_t squished_mallard_magic[] = { 0xD9, 0x00 };
static const char squished_mallard_name[] = "Duck";
static const uint16_t squished_mallard_usermem = 0x9C37;

// TI-73 nostub programs, run by the built-in assembly support of OS 1.40+,
// start with the tokenized "prgm0000" + type ('0' = unsquished hex text,
// '1' = squished raw binary) + 3 decimal digits for the size + hard newline.
static const uint8_t squished_nostub_prefix[] = { 0x5F, 0x30, 0x30, 0x30, 0x30 };

// TI-83 "Send(9prgm<name>" (tokenized BASIC, as used by the Venus family) and
// TI-83+ "Asm prgm<name>" (tokenized BASIC) shell headers, as found in Ion v1.6
// and PSE ("IONZ" / "PSEZ" followed by 3F D9 3F = "⏎Stop⏎", and a trailing 2A)
// and in Ice and SOS ("ZICE" / "ZSOSL" followed by 3F E1 = "⏎ClLCD"); the header
// embeds the shell's fixed name, not the program's, and carries no description.
static const uint8_t squished_send9_prefix[] = { 0xE7, 0x39, 0x5F };
static const uint8_t squished_asm_prefix[] = { 0xBB, 0x6A, 0x5F };
static const uint8_t squished_tail_stop[] = { 0x3F, 0xD9, 0x3F, 0x2A };
static const uint8_t squished_tail_cllcd[] = { 0x3F, 0xE1 };
static const char squished_ion83_name[] = "IONZ";
static const char squished_pse83_name[] = "PSEZ";
static const char squished_ice_name[] = "ZICE";
static const char squished_sos_name[] = "ZSOSL";

// TI-83+ TSE programs start with [BB 6D][C9]["TSE"][01][title][00][.dw memreq][code];
// the title is an ASCIIZ string which must begin with a space, and the 2-byte
// field holds the amount of external data (task block) the program needs.
static const uint8_t squished_tse_magic[] = { 0xC9, 'T', 'S', 'E', 0x01 };

// TI-83+ MirageOS programs start with [BB 6D][C9][01][30-byte 15x15 icon]["desc"][00][code];
// the modified format ([C9][03][icon][.dw quitaddr]["desc"][00]) and the external
// interface format ([C9][02][ID][xcoord]["desc"][00], which shares its prefix
// with the [C9][02] Ion module format) are only recognized on unsquish.
static const uint8_t squished_mirage_magic[] = { 0xC9, 0x01 };
static const uint8_t squished_mirage_modified[] = { 0xC9, 0x03 };
static const uint8_t squished_ion_module[] = { 0xC9, 0x02 };
static const uint32_t squished_mirage_iconlen = 30;

// TI-83 SOS modules start with the 2-byte "standard module header" (00 01).
static const uint8_t squished_sos_module[] = { 0x00, 0x01 };

// TI-82 squished programs start with a shell-specific marker followed by the
// shell's long name, NUL-terminated (as used by CrASH, ACE, ASH and SNG).
static const uint8_t * ti82_squished_prefix(int shell, uint32_t * prefix_length)
{
	switch (shell) {
		case SHELL_82_ACE: *prefix_length = sizeof(squished_prefix_82_ace);   return squished_prefix_82_ace;
		case SHELL_82_ASH: *prefix_length = sizeof(squished_prefix_82_ash);   return squished_prefix_82_ash;
		case SHELL_82_SNG: *prefix_length = sizeof(squished_prefix_82_sng);   return squished_prefix_82_sng;
		default:           *prefix_length = sizeof(squished_prefix_82_crash); return squished_prefix_82_crash;
	}
}

// Strip the TI-82 marker and long name, if present; return 1 if something was stripped.
static int ti82_strip_marker_and_name(char *** argv, const uint8_t ** ptr, uint32_t * datalen)
{
	static const uint8_t * const markers[4] = { squished_prefix_82_crash, squished_prefix_82_ace, squished_prefix_82_ash, squished_prefix_82_sng };
	unsigned int k;
	uint32_t namelen;

	for (k = 0; k < 4; k++) {
		if (data_has_prefix(*ptr, *datalen, markers[k], 3)) {
			*ptr += 3;
			*datalen -= 3;
			namelen = 0;
			while (namelen < *datalen && (*ptr)[namelen] != 0) {
				namelen++;
			}
			if (namelen < *datalen) {
				*ptr += namelen + 1;
				*datalen -= namelen + 1;
			}
			else {
				tifileutil_warning(_("%s: unterminated long name, passing it through."), (*argv)[0]);
			}
			return 1;
		}
	}
	return 0;
}

// Strip an Ion header: optional [BB][XX] marker, then [C9|AF][30 XX][description][00],
// where the description is NUL-terminated and the 30 XX is the jr nc offset covering
// it (and possibly more code before the shell entry point, as in Rogue); return 1
// if something was stripped.
static int ion_strip(const uint8_t ** ptr, uint32_t * datalen)
{
	const uint8_t * ip = *ptr;
	uint32_t ilen = *datalen;

	if (ilen >= 6 && ip[0] == 0xBB && (ip[2] == 0xC9 || ip[2] == 0xAF) && ip[3] == 0x30) {
		ip += 2;
		ilen -= 2;
	}
	if (ilen >= 4 && (ip[0] == 0xC9 || ip[0] == 0xAF) && ip[1] == 0x30) {
		const uint8_t * nul = (const uint8_t *)memchr(ip + 3, 0, ilen - 3);
		if (nul && (uint32_t)(nul - ip) < (uint32_t)(3 + ip[2])) {
			uint32_t skip = (uint32_t)(nul - ip) + 1;
			*ptr = nul + 1;
			*datalen = ilen - skip;
			return 1;
		}
	}
	return 0;
}

// Strip a Mallard header: [D9 00][4-char name][2-byte entry point][description][00];
// return 1 if something was stripped.
static int mallard_strip(const uint8_t ** ptr, uint32_t * datalen)
{
	if (*datalen >= 8 && data_has_prefix(*ptr, *datalen, squished_mallard_magic, sizeof(squished_mallard_magic))) {
		const uint8_t * nul = (const uint8_t *)memchr((*ptr) + 8, 0, *datalen - 8);
		if (nul) {
			uint32_t skip = (uint32_t)(nul - *ptr) + 1;
			*ptr += skip;
			*datalen -= skip;
			return 1;
		}
	}
	return 0;
}

// Strip a TI-73 nostub header: [prgm0000][type][3 decimal digits][3F]; return 1
// if something was stripped, and report the type ('0' = hex text, '1' = binary).
static int nostub_strip(const uint8_t ** ptr, uint32_t * datalen, uint8_t * type)
{
	if (   *datalen >= 10
	    && data_has_prefix(*ptr, *datalen, squished_nostub_prefix, sizeof(squished_nostub_prefix))
	    && ((*ptr)[5] == '0' || (*ptr)[5] == '1')
	    && (*ptr)[6] >= '0' && (*ptr)[6] <= '9'
	    && (*ptr)[7] >= '0' && (*ptr)[7] <= '9'
	    && (*ptr)[8] >= '0' && (*ptr)[8] <= '9'
	    && (*ptr)[9] == 0x3F) {
		if (type) {
			*type = (*ptr)[5];
		}
		*ptr += 10;
		*datalen -= 10;
		return 1;
	}
	return 0;
}

// Return the shell's fixed name and the header tail for the TI-83 "Send(9prgm"
// / TI-83+ "Asm prgm" shell families.
static const char * ti83x_shell_name(int shell, const uint8_t ** tail, uint32_t * tail_length)
{
	switch (shell) {
		case SHELL_83_ION83: *tail = squished_tail_stop; *tail_length = sizeof(squished_tail_stop); return squished_ion83_name;
		case SHELL_83_PSE83: *tail = squished_tail_stop; *tail_length = sizeof(squished_tail_stop); return squished_pse83_name;
		case SHELL_83_ICE:   *tail = squished_tail_cllcd; *tail_length = sizeof(squished_tail_cllcd); return squished_ice_name;
		default:             *tail = squished_tail_cllcd; *tail_length = sizeof(squished_tail_cllcd); return squished_sos_name;
	}
}

// Strip a TI-83 "Send(9prgm<name>⏎<tail>" or TI-83+ "Asm prgm<name>⏎<tail>"
// shell header, if present; return 1 if something was stripped.
static int ti83x_strip_shell(int model, const uint8_t ** ptr, uint32_t * datalen)
{
	static const struct { const char * name; const uint8_t * tail; uint32_t tail_length; } shells[4] = {
		{ squished_ion83_name, squished_tail_stop, sizeof(squished_tail_stop) },
		{ squished_pse83_name, squished_tail_stop, sizeof(squished_tail_stop) },
		{ squished_ice_name,   squished_tail_cllcd, sizeof(squished_tail_cllcd) },
		{ squished_sos_name,   squished_tail_cllcd, sizeof(squished_tail_cllcd) },
	};
	const uint8_t * prefix = (model == CALC_TI83) ? squished_send9_prefix : squished_asm_prefix;
	uint32_t prefix_length = sizeof(squished_send9_prefix);
	unsigned int k;

	for (k = 0; k < 4; k++) {
		uint32_t namelen = strlen(shells[k].name);
		uint32_t hlen = prefix_length + namelen + shells[k].tail_length;
		if (   data_has_prefix(*ptr, *datalen, prefix, prefix_length)
		    && data_has_prefix(*ptr + prefix_length, *datalen - prefix_length, (const uint8_t *)shells[k].name, namelen)
		    && data_has_prefix(*ptr + prefix_length + namelen, *datalen - prefix_length - namelen, shells[k].tail, shells[k].tail_length)) {
			*ptr += hlen;
			*datalen -= hlen;
			return 1;
		}
	}
	return 0;
}

// Strip a TI-83+ TSE header: [C9]["TSE"][01][title][00][.dw memreq]; return 1
// if something was stripped.
static int tse_strip(const uint8_t ** ptr, uint32_t * datalen)
{
	if (*datalen >= sizeof(squished_tse_magic) + 3 && data_has_prefix(*ptr, *datalen, squished_tse_magic, sizeof(squished_tse_magic))) {
		const uint8_t * nul = (const uint8_t *)memchr((*ptr) + sizeof(squished_tse_magic), 0, *datalen - sizeof(squished_tse_magic));
		if (nul && (uint32_t)(nul - *ptr) + 3 <= *datalen) {
			uint32_t skip = (uint32_t)(nul - *ptr) + 3;
			*ptr += skip;
			*datalen -= skip;
			return 1;
		}
	}
	return 0;
}

// Strip a TI-83+ MirageOS header ([C9][01][30-byte icon]["desc"][00], the
// modified [C9][03][icon][.dw quit]["desc"][00], or the external interface
// [C9][02][ID][x]["desc"][00]) or the [C9][02] Ion module prefix; return 1 if
// something was stripped. The Ion module interpretation wins for [C9][02],
// since the module format is far more common than the MirageOS interface one.
static int ti83p_strip_shell(const uint8_t ** ptr, uint32_t * datalen)
{
	uint32_t skip = 0;

	if (*datalen >= 2 + squished_mirage_iconlen + 2 && data_has_prefix(*ptr, *datalen, squished_mirage_magic, sizeof(squished_mirage_magic))) {
		const uint8_t * nul = (const uint8_t *)memchr((*ptr) + 2 + squished_mirage_iconlen, 0, *datalen - 2 - squished_mirage_iconlen);
		if (nul) {
			skip = (uint32_t)(nul - *ptr) + 1;
		}
	}
	else if (*datalen >= 2 + squished_mirage_iconlen + 2 + 2 && data_has_prefix(*ptr, *datalen, squished_mirage_modified, sizeof(squished_mirage_modified))) {
		const uint8_t * nul = (const uint8_t *)memchr((*ptr) + 2 + squished_mirage_iconlen + 2, 0, *datalen - 2 - squished_mirage_iconlen - 2);
		if (nul) {
			skip = (uint32_t)(nul - *ptr) + 1;
		}
	}
	else if (data_has_prefix(*ptr, *datalen, squished_ion_module, sizeof(squished_ion_module))) {
		skip = sizeof(squished_ion_module);
	}
	if (skip) {
		*ptr += skip;
		*datalen -= skip;
		return 1;
	}
	return 0;
}

// Strip a TI-83 SOS module header: [00 01]; return 1 if something was stripped.
static int sos_module_strip(const uint8_t ** ptr, uint32_t * datalen)
{
	if (data_has_prefix(*ptr, *datalen, squished_sos_module, sizeof(squished_sos_module))) {
		*ptr += sizeof(squished_sos_module);
		*datalen -= sizeof(squished_sos_module);
		return 1;
	}
	return 0;
}

// Strip the TI-83 shell-specific header, if present (ASHELL83 or Venus);
// return 1 if something was stripped.
static int ti83_strip_shell(const uint8_t ** ptr, uint32_t * datalen)
{
	// ASHELL83: [00][18 XX][00 00][Title ptr][Icon ptr][00 00][code...][title...]; the
	// jr offset leads to the code, and the title is reached through its absolute
	// pointer ($9327 + file offset).
	if (*datalen >= 7 && (*ptr)[0] == 0x00 && (*ptr)[1] == 0x18) {
		uint32_t codeoff = 3 + (*ptr)[2];
		uint32_t titleptr = (*ptr)[5] | ((uint32_t)(*ptr)[6] << 8);
		// The pointer is relative to the file start; subtract the 2-byte length field
		// and the header to get the offset relative to the stripped data.
		uint32_t titleoff = (titleptr >= 0x9327 + 0x48) ? (titleptr - 0x9327 - 0x48 - 2 - codeoff) : 0;

		if (codeoff > *datalen) {
			return 0;
		}
		*ptr += codeoff;
		*datalen -= codeoff;
		if (titleoff < *datalen) {
			uint32_t namelen = 0;
			while (namelen < *datalen - titleoff && (*ptr)[titleoff + namelen] != 0) {
				namelen++;
			}
			if (namelen < *datalen - titleoff) {
				uint32_t after = titleoff + namelen + 1;
				memmove((uint8_t *)(*ptr) + titleoff, (*ptr) + after, *datalen - after);
				*datalen -= after - titleoff;
			}
		}
		return 1;
	}
	// Venus: [magic "ç9_[V?"][00] optionally followed by [30 XX][description][00][code],
	// or by the code directly; the description is NUL-terminated and XX is the jr
	// nc offset covering it (and possibly more code before the shell entry point).
	if (*datalen >= sizeof(squished_venus_magic) && data_has_prefix(*ptr, *datalen, squished_venus_magic, sizeof(squished_venus_magic))) {
		*ptr += sizeof(squished_venus_magic);
		*datalen -= sizeof(squished_venus_magic);
		if (*datalen >= 3 && (*ptr)[0] == 0x30) {
			const uint8_t * nul = (const uint8_t *)memchr((*ptr) + 2, 0, *datalen - 2);
			if (nul && (uint32_t)(nul - *ptr) < (uint32_t)(2 + (*ptr)[1])) {
				uint32_t skip = (uint32_t)(nul - *ptr) + 1;
				*ptr += skip;
				*datalen -= skip;
			}
		}
		return 1;
	}
	// The "Send(9prgm<name>⏎<tail>" headers of the Ion v1.6, PSE, Ice and SOS shells.
	if (ti83x_strip_shell(CALC_TI83, ptr, datalen)) {
		return 1;
	}
	// The [C9][02] Ion module prefix.
	if (data_has_prefix(*ptr, *datalen, squished_ion_module, sizeof(squished_ion_module))) {
		*ptr += sizeof(squished_ion_module);
		*datalen -= sizeof(squished_ion_module);
		return 1;
	}
	// The [00 01] SOS module prefix.
	if (sos_module_strip(ptr, datalen)) {
		return 1;
	}
	return 0;
}

static int squish_model_supported(CalcModel model)
{
	return (   model == CALC_TI73 || model == CALC_TI82 || model == CALC_TI83
	        || model == CALC_TI83P || model == CALC_TI84P || model == CALC_TI84P_USB || model == CALC_TI82A_USB || model == CALC_TI84PT_USB
	        || model == CALC_TI84PC || model == CALC_TI84PC_USB
	        || model == CALC_TI83PCE_USB || model == CALC_TI84PCE_USB);
}

static const uint8_t * squished_prefix(CalcModel model, uint32_t * prefix_length)
{
	if (model == CALC_TI73 || model == CALC_TI83) {
		*prefix_length = 0;
		return nullptr;
	}
	if (model == CALC_TI82) {
		*prefix_length = sizeof(squished_prefix_82_crash);
		return squished_prefix_82_crash;
	}
	if (model == CALC_TI83P || model == CALC_TI84P) {
		*prefix_length = sizeof(squished_prefix_83p);
		return squished_prefix_83p;
	}
	if (model == CALC_TI84PC) {
		*prefix_length = sizeof(squished_prefix_cse);
		return squished_prefix_cse;
	}
	*prefix_length = sizeof(squished_prefix_ce);
	return squished_prefix_ce;
}

static int squish_uses_trailer(CalcModel model)
{
	return (model == CALC_TI73 || model == CALC_TI83);
}

static int tifileutil_squish_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s squish [OPTIONS] FILE
Where OPTIONS may include:
 -o OUTFILE:    output result to OUTFILE (required)
 -s SHELL:      target shell - TI-82: crash (default), ace, ash, sng;
                TI-73: mallard, nostub (default: no shell);
                TI-83: ion, ashell, venus, ion83, pse83, ice, sos (default: no shell);
                TI-83+/84+/CSE/CE: ion, ion83, pse83, ice, tse, mirage
 -L NAME:       set the shell program description (default: variable name)
 -v:            be verbose
Note: all options must precede the file name(s).
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_squish(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	const char * shellstr = nullptr;
	const char * longname = nullptr;
	int shell = SHELL_NONE;
	int verbose = 0;
	CalcModel model;
	FileContent * fc = nullptr;
	VarEntry * ve;
	const uint8_t * prefix;
	uint32_t prefix_length;
	uint32_t binarylen;
	const uint8_t * ptr;
	uint32_t datalen = 0;
	uint8_t * data = nullptr;
	uint32_t i;
	unsigned int first_file = 0;
	uint32_t namelen;
	uint32_t totallen;
	uint8_t * newdata;

	int j;
	// Skip argv[1], the command name.
	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_squish_usage);
					break;

					case 's':
						GET_OPTION_ARG(shellstr, "-s", tifileutil_squish_usage);
					break;

					case 'L':
						GET_OPTION_ARG(longname, "-L", tifileutil_squish_usage);
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_squish_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}

	if (!first_file) {
		return tifileutil_squish_usage(argc, argv);
	}
	infilename = (const char *)(*argv)[first_file];
	if (!outfilename) {
		tifileutil_critical(_("%s: no output file specified."), (*argv)[0]);
		return tifileutil_squish_usage(argc, argv);
	}

	if (shellstr) {
		if (!strcasecmp(shellstr, "crash")) {
			shell = SHELL_82_CRASH;
		}
		else if (!strcasecmp(shellstr, "ace")) {
			shell = SHELL_82_ACE;
		}
		else if (!strcasecmp(shellstr, "ash")) {
			shell = SHELL_82_ASH;
		}
		else if (!strcasecmp(shellstr, "sng")) {
			shell = SHELL_82_SNG;
		}
		else if (!strcasecmp(shellstr, "ion")) {
			shell = SHELL_83_ION;
		}
		else if (!strcasecmp(shellstr, "ashell")) {
			shell = SHELL_82_ASHELL;
		}
		else if (!strcasecmp(shellstr, "venus")) {
			shell = SHELL_83_VENUS;
		}
		else if (!strcasecmp(shellstr, "mallard")) {
			shell = SHELL_73_MALLARD;
		}
		else if (!strcasecmp(shellstr, "nostub")) {
			shell = SHELL_73_NOSTUB;
		}
		else if (!strcasecmp(shellstr, "ion83")) {
			shell = SHELL_83_ION83;
		}
		else if (!strcasecmp(shellstr, "pse83")) {
			shell = SHELL_83_PSE83;
		}
		else if (!strcasecmp(shellstr, "ice")) {
			shell = SHELL_83_ICE;
		}
		else if (!strcasecmp(shellstr, "sos")) {
			shell = SHELL_83_SOS;
		}
		else if (!strcasecmp(shellstr, "tse")) {
			shell = SHELL_83P_TSE;
		}
		else if (!strcasecmp(shellstr, "mirage")) {
			shell = SHELL_83P_MIRAGE;
		}
		else {
			tifileutil_critical(_("%s: unknown shell %s (expected TI-82: crash, ace, ash, sng; TI-73: mallard, nostub; TI-83: ion, ashell, venus, ion83, pse83, ice, sos; TI-83+/84+/CSE/CE: ion, ion83, pse83, ice, tse, mirage)."), (*argv)[0], shellstr);
			return tifileutil_squish_usage(argc, argv);
		}
	}

	model = tifiles_file_get_model(infilename);
	if (!model || !squish_model_supported(model)) {
		ret = 3;
		tifileutil_critical(_("%s: %s is not a supported TI-Z80 file."), (*argv)[0], infilename);
		goto end;
	}

	// Validate the shell selection against the model, and apply the defaults.
	if (model == CALC_TI82) {
		if (shell == SHELL_NONE) {
			shell = SHELL_82_CRASH;
		}
		else if (shell > SHELL_82_SNG) {
			ret = 1;
			tifileutil_critical(_("%s: the %s shell is for TI-73/83/83+, not TI-82."), (*argv)[0], shellstr);
			return tifileutil_squish_usage(argc, argv);
		}
	}
	else if (model == CALC_TI83) {
		if (shell != SHELL_NONE && !((shell >= SHELL_83_ION && shell <= SHELL_83_VENUS) || (shell >= SHELL_83_ION83 && shell <= SHELL_83_SOS))) {
			ret = 1;
			tifileutil_critical(_("%s: the %s shell is for TI-82, TI-73 or TI-83+, not TI-83."), (*argv)[0], shellstr);
			return tifileutil_squish_usage(argc, argv);
		}
	}
	else if (model == CALC_TI73) {
		// TI-73: the Mallard shell, or the shell-less nostub format run by the
		// built-in assembly support of OS 1.40+.
		if (shell != SHELL_NONE && shell != SHELL_73_MALLARD && shell != SHELL_73_NOSTUB) {
			ret = 1;
			tifileutil_critical(_("%s: the %s shell is for TI-82, TI-83 or TI-83+, not TI-73."), (*argv)[0], shellstr);
			return tifileutil_squish_usage(argc, argv);
		}
	}
	else if (shell != SHELL_NONE && shell != SHELL_83_ION
	      && !((shell >= SHELL_83_ION83 && shell <= SHELL_83_ICE) || shell == SHELL_83P_TSE || shell == SHELL_83P_MIRAGE)) {
		// The TI-83+ family carries the ION shell (with the model marker) and the
		// "Asm prgm" variants of the Ion v1.6, PSE and Ice shells, plus TSE and MirageOS.
		ret = 1;
		tifileutil_critical(_("%s: the %s shell is for TI-82, TI-73 or TI-83, not this model."), (*argv)[0], shellstr);
		return tifileutil_squish_usage(argc, argv);
	}
	if (longname && shell == SHELL_NONE) {
		ret = 1;
		tifileutil_critical(_("%s: -L requires a shell."), (*argv)[0]);
		return tifileutil_squish_usage(argc, argv);
	}
	if (longname && (shell == SHELL_73_NOSTUB || (shell >= SHELL_83_ION83 && shell <= SHELL_83_SOS))) {
		ret = 1;
		tifileutil_critical(_("%s: -L cannot be combined with -s %s, whose header carries no description."), (*argv)[0], shellstr);
		return tifileutil_squish_usage(argc, argv);
	}

	fc = tifiles_content_create_regular(model);
	if (!fc) {
		ret = 3;
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
		ret = 3;
		tifileutil_critical(_("%s: multi-entry file not supported."), (*argv)[0]);
		goto end;
	}
	ve = fc->entries[0];
	if (ve->type != 0x05 && ve->type != 0x06) {
		ret = 3;
		tifileutil_critical(_("%s: variable type 0x%02X is not a program type."), (*argv)[0], ve->type);
		goto end;
	}

	// The program data starts with a 2-byte length field, the length of the rest
	// (or, in some files, of the whole data); skip it if present.
	ptr = ve->data;
	datalen = ve->size;
	if ((datalen >= 2 && (uint32_t)(ptr[0] | ((uint16_t)ptr[1] << 8)) == datalen - 2)
	 || (datalen >= 2 && (uint32_t)(ptr[0] | ((uint16_t)ptr[1] << 8)) == datalen)) {
		ptr += 2;
		datalen -= 2;
	}

	// Strip a trailing "End 0000 End" trailer, if any.
	if (squish_uses_trailer(model) && datalen >= sizeof(squish_trailer)
	 && !memcmp(ptr + datalen - sizeof(squish_trailer), squish_trailer, sizeof(squish_trailer))) {
		datalen -= sizeof(squish_trailer);
	}

	// Strip a leading TI-73 nostub header, if any; the payload of an unsquished
	// nostub program is hex text, that of a squished one is raw binary.
	if (model == CALC_TI73) {
		uint8_t nostub_type = 0;
		if (nostub_strip(&ptr, &datalen, &nostub_type) && nostub_type == '1') {
			ret = 3;
			tifileutil_critical(_("%s: the TI-73 nostub program contains squished binary data, not hex text."), (*argv)[0]);
			goto end;
		}
	}

	// Convert hex text to binary, tolerating mixed case.
	if (datalen % 2) {
		ret = 3;
		tifileutil_critical(_("%s: odd number of hex digits."), (*argv)[0]);
		goto end;
	}
	data = (uint8_t *)malloc(datalen / 2);
	if (!data) {
		ret = 3;
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	for (i = 0; i < datalen; i += 2) {
		int hi = hex_nibble(ptr[i]);
		int lo = hex_nibble(ptr[i + 1]);
		if (hi < 0 || lo < 0) {
			ret = 3;
			tifileutil_critical(_("%s: non-hexadecimal data at offset %u."), (*argv)[0], i);
			goto end;
		}
		data[i / 2] = (hi << 4) | lo;
	}
	binarylen = datalen / 2;

	// The squished form is the binary code, prefixed with the model-specific marker
	// (and, on TI-82 and TI-83, the shell's long name or description) and the 2-byte
	// length field.
	prefix = squished_prefix(model, &prefix_length);

	namelen = (longname != nullptr) ? strlen(longname) : strlen(ve->name);

	if (model == CALC_TI82) {
		// TI-82: [marker][long name][00][code].
		prefix = ti82_squished_prefix(shell, &prefix_length);
		totallen = 2 + prefix_length + namelen + 1 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, prefix, prefix_length);
		memcpy(newdata + 2 + prefix_length, (longname != nullptr) ? longname : ve->name, namelen);
		newdata[2 + prefix_length + namelen] = 0;
		memcpy(newdata + 3 + prefix_length + namelen, data, binarylen);
	}
	else if (shell == SHELL_83_ION) {
		// ION: [model marker?][C9][30 len][description][00][code]; the length byte
		// covers the description and its NUL terminator, and doubles as the jr
		// offset. The marker is absent on TI-73/83, model-specific on TI-83+.
		totallen = 2 + prefix_length + 3 + namelen + 1 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		if (prefix_length) {
			memcpy(newdata + 2, prefix, prefix_length);
		}
		newdata[2 + prefix_length] = 0xC9;
		newdata[3 + prefix_length] = 0x30;
		newdata[4 + prefix_length] = namelen + 1;
		memcpy(newdata + 5 + prefix_length, (longname != nullptr) ? longname : ve->name, namelen);
		newdata[5 + prefix_length + namelen] = 0;
		memcpy(newdata + 6 + prefix_length + namelen, data, binarylen);
	}
	else if (model == CALC_TI83 && shell == SHELL_82_ASHELL) {
		// ASHELL83: [00][18 04][00 00][Title ptr][code][title][00]; the title pointer
		// is the absolute address of the title ($9327 + file offset).
		uint32_t titleptr = 0x9327 + 0x48 + 2 + 7 + binarylen;
		totallen = 2 + 7 + binarylen + namelen + 1;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		newdata[2] = 0x00;
		newdata[3] = 0x18;
		newdata[4] = 0x04; // jr over the 4 header bytes.
		newdata[5] = 0x00;
		newdata[6] = 0x00;
		newdata[7] = titleptr & 0xff;
		newdata[8] = (titleptr >> 8) & 0xff;
		memcpy(newdata + 9, data, binarylen);
		memcpy(newdata + 9 + binarylen, (longname != nullptr) ? longname : ve->name, namelen);
		newdata[9 + binarylen + namelen] = 0;
	}
	else if (model == CALC_TI83 && shell == SHELL_83_VENUS) {
		// Venus: [magic "ç9_[V?"][00][30 XX][description][00][code] (starts with the Send(9prgm...) token).
		// The jr offset covers the description and its NUL terminator.
		totallen = 2 + sizeof(squished_venus_magic) + 2 + namelen + 1 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, squished_venus_magic, sizeof(squished_venus_magic));
		newdata[2 + sizeof(squished_venus_magic)] = 0x30;
		newdata[3 + sizeof(squished_venus_magic)] = namelen + 1;
		memcpy(newdata + 4 + sizeof(squished_venus_magic), (longname != nullptr) ? longname : ve->name, namelen);
		newdata[4 + sizeof(squished_venus_magic) + namelen] = 0;
		memcpy(newdata + 5 + sizeof(squished_venus_magic) + namelen, data, binarylen);
	}
	else if (model == CALC_TI73 && shell == SHELL_73_MALLARD) {
		// Mallard: [D9 00]["Duck"][entry point][description][00][code]; the
		// entry point is the address of the code start, at userMem plus the
		// length of the header proper.
		totallen = 2 + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1 + 2 + namelen + 1 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, squished_mallard_magic, sizeof(squished_mallard_magic));
		memcpy(newdata + 2 + sizeof(squished_mallard_magic), squished_mallard_name, sizeof(squished_mallard_name) - 1);
		uint16_t entry = squished_mallard_usermem + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1 + 2 + namelen + 1;
		newdata[2 + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1] = entry & 0xff;
		newdata[3 + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1] = (entry >> 8) & 0xff;
		memcpy(newdata + 4 + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1, (longname != nullptr) ? longname : ve->name, namelen);
		newdata[4 + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1 + namelen] = 0;
		memcpy(newdata + 5 + sizeof(squished_mallard_magic) + sizeof(squished_mallard_name) - 1 + namelen, data, binarylen);
	}
	else if (model == CALC_TI73 && shell == SHELL_73_NOSTUB) {
		// Nostub: [prgm0000]['1'][3 decimal size digits][3F][code]; the size is
		// the length of the code in decimal, and the program must be protected.
		if (binarylen > 999) {
			ret = 3;
			tifileutil_critical(_("%s: %u bytes of binary data are too large for the TI-73 nostub format (max 999 bytes)."), (*argv)[0], binarylen);
			goto end;
		}
		if (binarylen > 900) {
			tifileutil_warning(_("%s: %u bytes of binary data are larger than the 900-byte limit of the TI-73 OS, the nostub program may not run."), (*argv)[0], binarylen);
		}
		totallen = 2 + sizeof(squished_nostub_prefix) + 1 + 3 + 1 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, squished_nostub_prefix, sizeof(squished_nostub_prefix));
		newdata[2 + sizeof(squished_nostub_prefix)] = '1';
		newdata[3 + sizeof(squished_nostub_prefix)] = '0' + (binarylen / 100) % 10;
		newdata[4 + sizeof(squished_nostub_prefix)] = '0' + (binarylen / 10) % 10;
		newdata[5 + sizeof(squished_nostub_prefix)] = '0' + binarylen % 10;
		newdata[6 + sizeof(squished_nostub_prefix)] = 0x3F;
		memcpy(newdata + 7 + sizeof(squished_nostub_prefix), data, binarylen);
	}
	else if (shell >= SHELL_83_ION83 && shell <= SHELL_83_SOS) {
		// TI-83 "Send(9prgm<name>⏎<tail>" / TI-83+ "Asm prgm<name>⏎<tail>" shell
		// headers, embedding the shell's fixed name (IONZ, PSEZ, ZICE or ZSOSL),
		// not the program's, and carrying no description.
		const char * shellname;
		const uint8_t * tail;
		uint32_t tail_length, shellnamelen;
		const uint8_t * family_prefix = (model == CALC_TI83) ? squished_send9_prefix : squished_asm_prefix;

		shellname = ti83x_shell_name(shell, &tail, &tail_length);
		shellnamelen = strlen(shellname);
		totallen = 2 + sizeof(squished_send9_prefix) + shellnamelen + tail_length + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, family_prefix, sizeof(squished_send9_prefix));
		memcpy(newdata + 2 + sizeof(squished_send9_prefix), shellname, shellnamelen);
		memcpy(newdata + 2 + sizeof(squished_send9_prefix) + shellnamelen, tail, tail_length);
		memcpy(newdata + 2 + sizeof(squished_send9_prefix) + shellnamelen + tail_length, data, binarylen);
	}
	else if (shell == SHELL_83P_TSE) {
		// TSE: [BB 6D][C9]["TSE"][01][" " + title][00][.dw 0][code]; the title must
		// begin with a space so that its first character is not corrupted.
		totallen = 2 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic) + 1 + namelen + 1 + 2 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, squished_prefix_83p, sizeof(squished_prefix_83p));
		memcpy(newdata + 2 + sizeof(squished_prefix_83p), squished_tse_magic, sizeof(squished_tse_magic));
		newdata[2 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic)] = ' ';
		memcpy(newdata + 3 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic), (longname != nullptr) ? longname : ve->name, namelen);
		newdata[3 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic) + namelen] = 0;
		newdata[4 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic) + namelen] = 0; // No external data required.
		newdata[5 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic) + namelen] = 0;
		memcpy(newdata + 6 + sizeof(squished_prefix_83p) + sizeof(squished_tse_magic) + namelen, data, binarylen);
	}
	else if (shell == SHELL_83P_MIRAGE) {
		// MirageOS: [BB 6D][C9][01][30-byte 15x15 icon]["desc"][00][code]; the icon
		// is blank, like in the official program template.
		totallen = 2 + sizeof(squished_prefix_83p) + sizeof(squished_mirage_magic) + squished_mirage_iconlen + namelen + 1 + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (totallen - 2) & 0xff;
		newdata[1] = ((totallen - 2) >> 8) & 0xff;
		memcpy(newdata + 2, squished_prefix_83p, sizeof(squished_prefix_83p));
		memcpy(newdata + 2 + sizeof(squished_prefix_83p), squished_mirage_magic, sizeof(squished_mirage_magic));
		memset(newdata + 2 + sizeof(squished_prefix_83p) + sizeof(squished_mirage_magic), 0, squished_mirage_iconlen);
		memcpy(newdata + 2 + sizeof(squished_prefix_83p) + sizeof(squished_mirage_magic) + squished_mirage_iconlen, (longname != nullptr) ? longname : ve->name, namelen);
		newdata[2 + sizeof(squished_prefix_83p) + sizeof(squished_mirage_magic) + squished_mirage_iconlen + namelen] = 0;
		memcpy(newdata + 3 + sizeof(squished_prefix_83p) + sizeof(squished_mirage_magic) + squished_mirage_iconlen + namelen, data, binarylen);
	}
	else {
		// Markerless (TI-73/83 default) or model-specific marker (TI-83+/84+/CSE/CE).
		totallen = 2 + prefix_length + binarylen;
		newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
		if (!newdata) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		newdata[0] = (prefix_length + binarylen) & 0xff;
		newdata[1] = ((prefix_length + binarylen) >> 8) & 0xff;
		if (prefix_length) {
			memcpy(newdata + 2, prefix, prefix_length);
		}
		memcpy(newdata + 2 + prefix_length, data, binarylen);
	}
	tifiles_ve_free_data(ve->data);
	ve->data = newdata;
	ve->size = totallen;

	free(data);
	data = nullptr;

	ve->type = 0x06; // Protected program (squished code must not be editable).

	ret = err_print(argv, "tifiles_file_write_regular", tifiles_file_write_regular(outfilename, fc, 0));
	if (!ret && verbose) {
		tifileutil_info(_("%s: wrote %u bytes of binary data to %s."), (*argv)[0], binarylen, outfilename);
	}

end:
	free(data);
	if (fc) {
		tifiles_content_delete_regular(fc);
	}

	return ret;
}

static int tifileutil_unsquish_usage(int * argc, char *** argv)
{
	fprintf(stderr, R"x(Usage: %s unsquish [OPTIONS] FILE
Where OPTIONS may include:
 -o OUTFILE:    output result to OUTFILE (- for stdout; required for TI-Z80 files,
                default stdout for TI-68k ASM -> Exec string)
 -v:            be verbose
Note: all options must precede the file name(s).
)x", (*argv)[0]);
	return 1;
}

static int tifileutil_unsquish(int * argc, char *** argv, unsigned int offset)
{
	int ret = 3;

	const char * infilename = nullptr;
	char * outfilename = nullptr;
	int verbose = 0;
	CalcModel model;
	FileContent * fc = nullptr;
	VarEntry * ve;
	uint32_t start = 0, datalen, prefix_length;
	const uint8_t * ptr;
	char * hex = nullptr;
	FILE * outfile = nullptr;
	uint32_t hexlen, trailerlen, totallen;
	uint8_t * newdata;
	uint8_t nostub_type = 0;

	unsigned int i;
	unsigned int first_file = 0;
	int j;

	// Skip argv[1], the command name.
	for (i = offset + 2; i < (unsigned int)*argc; i++) {
		if ((*argv)[i][0] == '-' && (*argv)[i][1]) {
			for (j = 1; (*argv)[i][j]; j++) {
				switch ((*argv)[i][j]) {
					case 'o':
						GET_OPTION_ARG(outfilename, "-o", tifileutil_unsquish_usage);
					break;

					case 'v':
						verbose = 1;
					break;

					default:
						tifileutil_critical(_("%s: unknown option -%c."), (*argv)[0], (*argv)[i][j]);
					return tifileutil_unsquish_usage(argc, argv);
				}
			}
		}
		else {
			first_file = i;
			break;
		}
	}

	if (!first_file) {
		return tifileutil_unsquish_usage(argc, argv);
	}
	infilename = (const char *)(*argv)[first_file];

	model = tifiles_file_get_model(infilename);
	if (!model) {
		ret = 3;
		tifileutil_critical(_("%s: invalid input file %s."), (*argv)[0], infilename);
		goto end;
	}

	fc = tifiles_content_create_regular(model);
	if (!fc) {
		ret = 3;
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
		ret = 3;
		tifileutil_critical(_("%s: multi-entry file not supported."), (*argv)[0]);
		goto end;
	}
	ve = fc->entries[0];

	if (ticonv_model_is_ti68k(model) && ve->type == TI89_ASM) {
		// TI-68k ASM programs: turn the code into a TI-BASIC Exec string, like ttasm2exec.pl.
		uint32_t end = ve->size;
		// The parsed entry data starts with a 2-byte size field, and the code proper
		// follows the relocation data and ends with a 0xF3 tag.
		if (start + 2 <= end) {
			start += 2;
		}
		if (end > start && ve->data[end - 1] == 0xF3) {
			end--;
		}
		else if (end > start) {
			tifileutil_warning(_("%s: no 0xF3 tag found, the Exec string is unlikely to work."), (*argv)[0]);
		}
		if (end <= start) {
			ret = 3;
			tifileutil_critical(_("%s: %s is too short to be a valid ASM file."), (*argv)[0], infilename);
			goto end;
		}
		hex = hex_encode(ve->data + start, end - start, 0);
		if (!hex) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		if (outfilename && !(outfilename[0] == '-' && outfilename[1] == 0)) {
			outfile = fopen(outfilename, "wt");
			if (!outfile) {
				perror(outfilename);
				ret = 2;
				goto end;
			}
		}
		else {
			outfile = stdout;
		}
		fprintf(outfile, "Exec \"%s\"\n", hex);
		if (verbose) {
			tifileutil_info(_("%s: wrote Exec string to %s."), (*argv)[0], outfile == stdout ? "stdout" : outfilename);
		}
		ret = 0;
		goto end;
	}

	if (!squish_model_supported(model)) {
		ret = 3;
		tifileutil_critical(_("%s: %s is not a supported TI-Z80 file."), (*argv)[0], infilename);
		goto end;
	}
	if (ve->type != 0x05 && ve->type != 0x06) {
		ret = 3;
		tifileutil_critical(_("%s: variable type 0x%02X is not a program type."), (*argv)[0], ve->type);
		goto end;
	}
	if (!outfilename) {
		tifileutil_critical(_("%s: no output file specified."), (*argv)[0]);
		return tifileutil_unsquish_usage(argc, argv);
	}

	// The program data starts with a 2-byte length field, the length of the rest
	// (or, in some files, of the whole data); skip it if present.
	ptr = ve->data;
	datalen = ve->size;
	if ((datalen >= 2 && (uint32_t)(ptr[0] | ((uint16_t)ptr[1] << 8)) == datalen - 2)
	 || (datalen >= 2 && (uint32_t)(ptr[0] | ((uint16_t)ptr[1] << 8)) == datalen)) {
		ptr += 2;
		datalen -= 2;
	}

	// Strip the model-specific squished marker, if any.
	{
		if (!((model == CALC_TI82 && ti82_strip_marker_and_name(argv, &ptr, &datalen))
		   || (model == CALC_TI83 && ti83_strip_shell(&ptr, &datalen))
		   || (model == CALC_TI73 && (mallard_strip(&ptr, &datalen) || nostub_strip(&ptr, &datalen, &nostub_type))))) {
			const uint8_t * prefix = squished_prefix(model, &prefix_length);
			if (prefix_length && data_has_prefix(ptr, datalen, prefix, prefix_length)) {
				ptr += prefix_length;
				datalen -= prefix_length;
			}
		}
	}

	// Strip the TI-83+ shell-specific headers, if any: the "Asm prgm<name>⏎<tail>"
	// family (with no model marker), the TSE header (after the model marker), and
	// the MirageOS and Ion module headers (after the model marker).
	if (model != CALC_TI73 && model != CALC_TI82 && model != CALC_TI83) {
		ti83x_strip_shell(CALC_TI83P, &ptr, &datalen);
		tse_strip(&ptr, &datalen);
		ti83p_strip_shell(&ptr, &datalen);
	}

	// TI-73/83/83+ family Ion programs carry a [C9][30 XX][description][00] header,
	// optionally preceded by the TI-83+ marker [BB 6D]; strip it if present.
	if (model != CALC_TI82 && datalen >= 4) {
		ion_strip(&ptr, &datalen);
	}

	// Strip a trailing "End 0000 End" trailer, if any.
	if (squish_uses_trailer(model) && datalen >= sizeof(squish_trailer)
	 && !memcmp(ptr + datalen - sizeof(squish_trailer), squish_trailer, sizeof(squish_trailer))) {
		datalen -= sizeof(squish_trailer);
	}

	// Convert binary to hex text, appending the 2-byte length field and the trailer on TI-73/83.
	if (nostub_type == '0') {
		// The payload of an unsquished TI-73 nostub program is already hex
		// text; pass it through instead of hex-encoding it again.
		hex = (char *)malloc(datalen + 1);
		if (!hex) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}
		memcpy(hex, ptr, datalen);
		hex[datalen] = 0;
		hexlen = datalen;
	}
	else {
		hex = hex_encode(ptr, datalen, 1);
		if (!hex) {
			ret = 3;
			tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
			goto end;
		}

		hexlen = 2 * datalen;
	}
	trailerlen = squish_uses_trailer(model) ? sizeof(squish_trailer) : 0;
	totallen = 2 + hexlen + trailerlen;
	newdata = (uint8_t *)tifiles_ve_alloc_data(totallen);
	if (!newdata) {
		ret = 3;
		tifileutil_critical(_("%s: cannot allocate memory."), (*argv)[0]);
		goto end;
	}
	newdata[0] = (hexlen + trailerlen) & 0xff;
	newdata[1] = ((hexlen + trailerlen) >> 8) & 0xff;
	memcpy(newdata + 2, hex, hexlen);
	if (trailerlen) {
		memcpy(newdata + 2 + hexlen, squish_trailer, trailerlen);
	}
	tifiles_ve_free_data(ve->data);
	ve->data = newdata;
	ve->size = totallen;

	free(hex);
	hex = nullptr;

	ve->type = 0x05; // Unprotected program (hex text must remain editable).

	ret = err_print(argv, "tifiles_file_write_regular", tifiles_file_write_regular(outfilename, fc, 0));
	if (!ret && verbose) {
		tifileutil_info(_("%s: wrote %u bytes of hex data to %s."), (*argv)[0], ve->size, outfilename);
	}

end:
	free(hex);
	if (outfile && outfile != stdout) {
		fclose(outfile);
	}
	if (fc) {
		tifiles_content_delete_regular(fc);
	}

	return ret;
}
