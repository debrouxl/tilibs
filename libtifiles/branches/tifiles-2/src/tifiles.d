
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
} TiCalcModel;

/**
 * TiFileAttr:
 *
 * An enumeration which contains the different variable attributes.
 **/
typedef enum 
{
	CALC_NONE = 0,
	CALC_TI73, CALC_TI82, CALC_TI83, CALC_TI83P, CALC_TI84P, CALC_TI85, CALC_TI86,
	CALC_TI89, CALC_TI89T, CALC_TI92, CALC_TI92P, CALC_V200,
} TiCalcModel;

/**
 * TiFileClass:
 *
 * An enumeration which contains the following class of TI files:
 * TIFILE_SINGLE = 1, TIFILE_GROUP = 2, TIFILE_BACKUP = 4, TIFILE_FLASH = 8
 **/

/**
 * TiFileEncoding:
 *
 * An enumeration which contains the following encodings:
 * ENCODING_ASCII = 1, ENCODING_LATIN1, ENCODING_UNICODE
 **/

/**
 * TiVarEntry:
 * @folder:
 * @name:
 * @trans:
 * @type:
 * @
 *
 * A generic structure used to store the content of a TI file.
 **/

