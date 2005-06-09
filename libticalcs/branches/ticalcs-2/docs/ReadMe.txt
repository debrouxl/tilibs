1) About folders:
-----------------

- tmpl: html template (style sheet, images) + hand-written doc
- html: html generated doc
- text: text generated doc


2) About comments:
------------------

Each function or definition starts with a specific comment header like this:

/**
 * tifiles_get_error:
 * @number: error number (see file_err.h for list).
 * @message: a newly allocated string which contains corresponding error *message.
 *
 * Attempt to match the message corresponding to the error number. The returned
 * string must be freed when no longer needed.
 *
 * Return value: 0 if error has been caught, the error number otherwise (propagation).
 **/
TIEXPORT int TICALL function
 
@parameter: this is a parameter
#function: html link to function
%name: report to parameter (written in italic)

Current restrictions: the documentation generator is not very well protected
against syntax errors. You must follow these rules:
- line with @ must be on 1 line (wrapping is allowed).
- function declaration must be on 1 line (same as above).
- the '*' _must_ be placed as showed.

3) About doc:
-------------

apiindex.html contains list of topics with anchor like this:
	[] Entry points (tifiles.c)
	
The title describes a set of functions. The parser will scan them in tifiles.c and will
build the tifiles.html file.

Once all files generated, it will modify the apiindex.html file for html link insertion.
	

3) About the documentation system:
----------------------------------

The processing is the following: each *.c or *.h source file is scanned for "/**" token up to
function or structure definition.

        *.[c|h] + tmpl -> html

The processor is a C-written program and works in 1 pass.

Romain.
-------