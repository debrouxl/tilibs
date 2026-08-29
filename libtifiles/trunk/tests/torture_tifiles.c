#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <glib.h>
#include <tifiles.h>
#include "../src/error.h"

#define PRINTF(FUNCTION, TYPE, ...) \
fprintf(stderr, "%d\t" TYPE "\n", __LINE__, FUNCTION(__VA_ARGS__))

#define PRINTFVOID(FUNCTION, ...) \
FUNCTION(__VA_ARGS__); fprintf(stderr, "%d\n", __LINE__)

#define INT "%d"
#define PTR "%p"
#define STR "\"%s\""

static const uint8_t certdata[] =
{
                                                                                    0x03, 0x0E,
0x02, 0x0A, 0x00, 0x10, 0x01, 0x01, 0x07, 0x04, 0x05, 0x01, 0x23, 0x45, 0x67, 0x89, 0x03, 0x26,
0x09, 0x04, 0x12, 0x34, 0x56, 0x78, 0x02, 0x0D, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x0E, 0x01, 0xAF, 0x07, 0x11, 0x04,
0x07, 0x3D, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,
0x3E, 0x3F, 0x40, 0x07, 0x12, 0x01, 0x04, 0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05,
0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15,
0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x12, 0x52, 0x04, 0x07,
0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D,
0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D,
0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D,
0x3E, 0x3F, 0x40, 0x07, 0x13, 0x03, 0x5F, 0x04, 0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04,
0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34,
0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x12, 0x32, 0x04,
0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C,
0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C,
0x3D, 0x3E, 0x3F, 0x40, 0x07, 0x12, 0x22, 0x04, 0x07, 0x3D, 0x41, 0x40, 0x01, 0x02, 0x03, 0x04,
0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24,
0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34,
0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x02, 0x0D, 0x40, 0x01,
0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11,
0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21,
0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31,
0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x40
};

static const uint16_t field_path1[] = { 0x0300, 0x0320, 0x0900 };
static const uint16_t field_path2[] = { 0x0300, 0x0700, 0x0730 };
static const uint16_t field_path3[] = { 0x0300, 0x0400, 0x0730 };

static void cert_functions_unit_test(void)
{
    uint16_t field_type;
    const uint8_t * contents = NULL;
    const uint8_t * data;
    uint32_t field_size;

    assert(ERR_INVALID_PARAM == tifiles_cert_field_get(NULL, 0x12345678, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_get((void *)0x12345678, 0x12345678, NULL, NULL, NULL));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_get((void *)0x12345678, 1, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_CERT_TRUNCATED == tifiles_cert_field_get(certdata, 3, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_CERT_TRUNCATED == tifiles_cert_field_get(certdata, 0x100, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678));
    assert(0 == tifiles_cert_field_get(certdata, sizeof(certdata) / sizeof(certdata[0]), &field_type, &contents, &field_size));
    assert(0x030E == field_type);
    assert(NULL != contents);
    assert(0x020A == field_size);

    data = certdata;
    field_size = sizeof(certdata) / sizeof(certdata[0]);
    assert(ERR_INVALID_PARAM == tifiles_cert_field_next(NULL, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_next((void *)0x12345678, NULL));
    assert(0 == tifiles_cert_field_next(&data, &field_size));
    assert(0 == tifiles_cert_field_get(data, field_size, &field_type, &contents, &field_size));
    assert(0x020D == field_type);
    assert(NULL != contents);
    assert(0x0040 == field_size);

    assert(ERR_INVALID_PARAM == tifiles_cert_field_find(NULL, 0x12345678, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find((void *)0x12345678, 1, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(0 == tifiles_cert_field_find(certdata, sizeof(certdata) / sizeof(certdata[0]), 0x300, &contents, &field_size));
    assert(certdata + 2 + 2 == contents);
    assert(0x020A == field_size);
    assert(0 == tifiles_cert_field_find(certdata, sizeof(certdata) / sizeof(certdata[0]), 0x30E, &contents, &field_size));
    assert(certdata + 2 + 2 == contents);
    assert(0x020A == field_size);
    assert(0 == tifiles_cert_field_find(certdata, sizeof(certdata) / sizeof(certdata[0]), 0x20D, &contents, &field_size));
    assert(certdata + 2 + 2 + 0x20A + 2 + 1 == contents);
    assert(0x0040 == field_size);

    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path(NULL, 0x12345678, (void *)0x12345678, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path((void *)0x12345678, 0x12345678, NULL, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path((void *)0x12345678, 1, NULL, 0x1234, (void *)0x12345678, (void *)0x12345678));
    assert(ERR_INVALID_PARAM == tifiles_cert_field_find_path((void *)0x12345678, 0x12345678, (void *)0x12345678, 0, (void *)0x12345678, (void *)0x12345678));
    assert(0 == tifiles_cert_field_find_path(certdata, sizeof(certdata) / sizeof(certdata[0]), field_path1, sizeof(field_path1) / sizeof(field_path1[0]), &contents, &field_size));
    assert(0x0004 == field_size);
    assert(0 == tifiles_cert_field_find_path(certdata, sizeof(certdata) / sizeof(certdata[0]), field_path2, sizeof(field_path2) / sizeof(field_path2[0]), &contents, &field_size));
    assert(0x0040 == field_size);
    assert(0 != tifiles_cert_field_find_path(certdata, sizeof(certdata) / sizeof(certdata[0]), field_path3, sizeof(field_path3) / sizeof(field_path3[0]), &contents, &field_size));
}

/* --- memreader / buffer-based classification tests ---------------------- */

static char g_tmpdir[4096];

static void make_tmpdir(void)
{
	snprintf(g_tmpdir, sizeof(g_tmpdir), "tifiles_memreader_test.%d", (int)getpid());
	if (mkdir(g_tmpdir, 0700) != 0)
	{
		g_tmpdir[0] = '.';
		g_tmpdir[1] = '\0';
	}
}

static void write_file(const char *name, const uint8_t *data, size_t n)
{
	char path[5000];
	FILE *f;

	snprintf(path, sizeof(path), "%s/%s", g_tmpdir, name);
	f = fopen(path, "wb");
	assert(f != NULL);
	if (n)
	{
		assert(fwrite(data, 1, n, f) == n);
	}
	assert(fclose(f) == 0);
}

static void classify(const char *path,
                     int *is_ti, int *is_single, int *is_group, int *is_regular,
                     int *is_backup, int *is_os, int *is_app, int *is_tib,
                     int *is_flash, int *is_tigroup, int *is_tno)
{
	*is_ti      = tifiles_file_is_ti(path);
	*is_single  = tifiles_file_is_single(path);
	*is_group   = tifiles_file_is_group(path);
	*is_regular = tifiles_file_is_regular(path);
	*is_backup  = tifiles_file_is_backup(path);
	*is_os      = tifiles_file_is_os(path);
	*is_app     = tifiles_file_is_app(path);
	*is_tib     = tifiles_file_is_tib(path);
	*is_flash   = tifiles_file_is_flash(path);
	*is_tigroup = tifiles_file_is_tigroup(path);
	*is_tno     = tifiles_file_is_tno(path);
}

static void test_null_and_invalid(void)
{
	TiMemReader r;

	assert(tifiles_file_is_ti(NULL) == 0);
	assert(tifiles_file_is_single(NULL) == 0);
	assert(tifiles_file_is_group(NULL) == 0);
	assert(tifiles_file_is_regular(NULL) == 0);
	assert(tifiles_file_is_backup(NULL) == 0);
	assert(tifiles_file_is_os(NULL) == 0);
	assert(tifiles_file_is_app(NULL) == 0);
	assert(tifiles_file_is_tib(NULL) == 0);
	assert(tifiles_file_is_flash(NULL) == 0);
	assert(tifiles_file_is_tigroup(NULL) == 0);
	assert(tifiles_file_is_tno(NULL) == 0);

	assert(tifiles_file_has_ti_header(NULL) == 0);
	assert(tifiles_file_has_tib_header(NULL) == 0);
	assert(tifiles_file_has_tig_header(NULL) == 0);
	assert(tifiles_file_has_tifl_header(NULL, NULL, NULL) == 0);
	assert(tifiles_file_has_tno_header(NULL) == 0);

	tifiles_mem_reader_init(&r, NULL, 0);
	assert(tifiles_mem_reader_create_from_file(&r, NULL, TIFILES_HEAD_READ_CAP) != 0);
	tifiles_mem_reader_destroy(&r);

	assert(tifiles_mem_reader_create_from_file(&r, "/this/does/not/exist/at/all.xyz", TIFILES_HEAD_READ_CAP) != 0);
	tifiles_mem_reader_destroy(&r);
}

static void test_small_and_empty_files(void)
{
	static const size_t sizes[] = { 0, 1, 2, 3, 4, 5, 8, 16, 63, 64, 100, 1000 };
	size_t i;
	uint8_t *buf = malloc(1000);
	assert(buf != NULL);
	for (i = 0; i < 1000; i++)
	{
		buf[i] = (uint8_t)(0xCD + i);
	}

	for (i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
	{
		char name[64];
		char path[5000];
		int is_ti, is_single, is_group, is_regular, is_backup, is_os;
		int is_app, is_tib, is_flash, is_tigroup, is_tno;
		TiMemReader r;
		size_t j;
		uint8_t b;

		snprintf(name, sizeof(name), "small_%zu.bin", sizes[i]);
		write_file(name, buf, sizes[i]);
		snprintf(path, sizeof(path), "%s/%s", g_tmpdir, name);

		classify(path, &is_ti, &is_single, &is_group, &is_regular,
		         &is_backup, &is_os, &is_app, &is_tib, &is_flash, &is_tigroup, &is_tno);
		assert(is_ti == 0);
		assert(is_single == 0);
		assert(is_group == 0);
		assert(is_regular == 0);
		assert(is_backup == 0);
		assert(is_os == 0);
		assert(is_app == 0);
		assert(is_tib == 0);
		assert(is_flash == 0);
		assert(is_tigroup == 0);
		assert(is_tno == 0);

		if (sizes[i] == 0)
		{
			tifiles_mem_reader_init(&r, NULL, 0);
			assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) != 0);
			tifiles_mem_reader_destroy(&r);
			continue;
		}

		assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
		assert(r.data != NULL);
		assert(r.size == sizes[i]);

		tifiles_mem_reader_init(&r, NULL, 0);
		assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
		for (j = 0; j < sizes[i]; j++)
		{
			assert(tifiles_mem_reader_read(&r, &b, 1) == 1);
			assert(b == buf[j]);
			assert(r.pos == j + 1);
		}
		assert(tifiles_mem_reader_read(&r, &b, 1) == 0);
		{
			uint8_t tmp[4];
			assert(tifiles_mem_reader_read(&r, tmp, 2) == 0);
			assert(tifiles_mem_reader_read(&r, tmp, 4) == 0);
		}
		assert(tifiles_mem_reader_seek(&r, r.size) == 0);
		assert(r.pos == sizes[i]);
		tifiles_mem_reader_destroy(&r);
	}

	free(buf);
}

static void test_large_file(void)
{
	static const size_t n = 2u * 1024u * 1024u;
	uint8_t *buf = malloc(n);
	TiMemReader r;
	size_t i;
	int is_ti, is_single, is_group, is_regular, is_backup, is_os;
	int is_app, is_tib, is_flash, is_tigroup, is_tno;
	uint8_t b;
	char path[5000];

	assert(buf != NULL);
	for (i = 0; i < n; i++)
	{
		buf[i] = (uint8_t)(i * 31 + 7);
	}
	write_file("large.bin", buf, n);
	snprintf(path, sizeof(path), "%s/%s", g_tmpdir, "large.bin");

	classify(path, &is_ti, &is_single, &is_group, &is_regular,
	         &is_backup, &is_os, &is_app, &is_tib, &is_flash, &is_tigroup, &is_tno);
	assert(is_ti == 0);
	assert(is_flash == 0);
	assert(is_tib == 0);
	assert(is_tigroup == 0);

	assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
	assert(r.data != NULL);
	/* Ordinary header detection only reads a small prefix of the file. */
	assert(r.size == TIFILES_HEAD_READ_CAP);
	assert(r.size < n);

	assert(tifiles_mem_reader_seek(&r, 0) == 0);
	assert(tifiles_mem_reader_read(&r, &b, 1) == 1 && b == buf[0]);
	assert(tifiles_mem_reader_seek(&r, TIFILES_HEAD_READ_CAP / 2) == 0);
	assert(tifiles_mem_reader_read(&r, &b, 1) == 1 && b == buf[TIFILES_HEAD_READ_CAP / 2]);
	assert(tifiles_mem_reader_seek(&r, r.size - 1) == 0);
	assert(tifiles_mem_reader_read(&r, &b, 1) == 1 && b == buf[r.size - 1]);
	/* Nothing beyond the prefix is available. */
	assert(tifiles_mem_reader_seek(&r, r.size) == 0);
	assert(tifiles_mem_reader_read(&r, &b, 1) == 0);
	tifiles_mem_reader_destroy(&r);
	free(buf);
}

/* Multi-entry TIFL files (old OS/app files that embed a license agreement or a
   certificate before the real OS/app) require the reader to walk past the first
   entry. For real devices that entry always starts beyond the first 4 KB page
   but within 4 MB, so these calls must read the whole (<=4 MB) file while
   ordinary header detection stays capped at one page. */
static void test_tifl_multi_entry(void)
{
	const uint8_t lic_type = 0x3e;   /* certificate/license, not OS/app */
	uint8_t e1[78], e2[78];
	uint8_t *file;
	size_t   lic_len, pay_len, total;
	char     path[5000];
	uint8_t  dev_type = 0, data_type = 0;

	memset(e1, 0, sizeof(e1));
	memset(e2, 0, sizeof(e2));
	memcpy(e1, "**TIFL**", 8); e1[8] = 0x01; e1[16] = 7; memcpy(e1 + 17, "License", 7);
	memcpy(e2, "**TIFL**", 8);                e2[16] = 8; memcpy(e2 + 17, "basecode", 8);

	lic_len = 6000;                  /* pushes the real entry past one page */

	/* 1) OS multi-entry: license, then an OS entry (data_type == AMS / 0x23). */
	e1[48] = 0x74; e1[49] = lic_type;
	e1[74] = (uint8_t)(lic_len & 0xff); e1[75] = (uint8_t)((lic_len >> 8) & 0xff);
	e1[76] = (uint8_t)((lic_len >> 16) & 0xff); e1[77] = (uint8_t)((lic_len >> 24) & 0xff);
	e2[48] = 0x98; e2[49] = TI83p_AMS;
	pay_len = 4096;
	e2[74] = (uint8_t)(pay_len & 0xff); e2[75] = (uint8_t)((pay_len >> 8) & 0xff);
	e2[76] = (uint8_t)((pay_len >> 16) & 0xff); e2[77] = (uint8_t)((pay_len >> 24) & 0xff);

	total = sizeof(e1) + lic_len + sizeof(e2) + pay_len;
	file = (uint8_t *)malloc(total);
	assert(file != NULL);
	memcpy(file, e1, sizeof(e1));
	memset(file + sizeof(e1), 0xAB, lic_len);
	memcpy(file + sizeof(e1) + lic_len, e2, sizeof(e2));
	memset(file + sizeof(e1) + lic_len + sizeof(e2), 0xCD, pay_len);
	write_file("tifl_os_multi.bin", file, total);
	snprintf(path, sizeof(path), "%s/%s", g_tmpdir, "tifl_os_multi.bin");

	assert(tifiles_file_is_os(path) == 1);
	assert(tifiles_file_has_tifl_header(path, &dev_type, &data_type) != 0);
	assert(data_type == TI83p_AMS);
	assert(dev_type == 0x98);
	assert(tifiles_file_is_app(path) == 0);

	/* Reading only the first 4 KB stops at the license entry and misses the OS,
	   which is exactly why these calls read the whole (<=4 MB) file. */
	{
		TiMemReader r;
		assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
		assert(tifiles_buffer_is_os(r.data, r.size) == 0);
		tifiles_mem_reader_destroy(&r);
	}
	free(file);

	/* 2) APP multi-entry: license, then an app entry (data_type == APPL / 0x24). */
	e2[49] = TI83p_APPL;
	e2[74] = (uint8_t)(pay_len & 0xff); e2[75] = (uint8_t)((pay_len >> 8) & 0xff);
	e2[76] = (uint8_t)((pay_len >> 16) & 0xff); e2[77] = (uint8_t)((pay_len >> 24) & 0xff);

	total = sizeof(e1) + lic_len + sizeof(e2) + pay_len;
	file = (uint8_t *)malloc(total);
	assert(file != NULL);
	memcpy(file, e1, sizeof(e1));
	memset(file + sizeof(e1), 0xAB, lic_len);
	memcpy(file + sizeof(e1) + lic_len, e2, sizeof(e2));
	memset(file + sizeof(e1) + lic_len + sizeof(e2), 0xCD, pay_len);
	write_file("tifl_app_multi.bin", file, total);
	snprintf(path, sizeof(path), "%s/%s", g_tmpdir, "tifl_app_multi.bin");

	assert(tifiles_file_is_app(path) == 1);
	assert(tifiles_file_has_tifl_header(path, &dev_type, &data_type) != 0);
	assert(data_type == TI83p_APPL);
	assert(tifiles_file_is_os(path) == 0);
	free(file);

	/* 3) Real PedroM OS contents behind a fake GPLv3 license (committed sample).
	   If the sample is absent, the in-memory cases above already cover the
	   multi-entry logic. */
	{
		static const char *cand[] =
		{
			"pedrom083_with_license.89u",
			"ti89/pedrom083_with_license.89u",
			"../ti89/pedrom083_with_license.89u",
			"../../ti89/pedrom083_with_license.89u",
			NULL
		};
		int k;
		for (k = 0; cand[k] != NULL; k++)
		{
			if (access(cand[k], R_OK) == 0)
			{
				assert(tifiles_file_is_os(cand[k]) == 1);
				break;
			}
		}
	}

	/* 4) Large Nspire OS file (> 4 MB) carrying its own magic (not **TIFL**):
	   it must be classified via buffer_has_tno_header() on the first page,
	   with no large read and no memory blowup. */
	{
		static const size_t big = 5u * 1024u * 1024u;
		uint8_t *ns = (uint8_t *)malloc(big);
		assert(ns != NULL);
		memcpy(ns, "TI-Nspire.tno ", 14);   /* bytes buffer_has_tno_header() checks */
		memset(ns + 14, 0x5E, big - 14);
		write_file("nspire_os_big.bin", ns, big);
		snprintf(path, sizeof(path), "%s/%s", g_tmpdir, "nspire_os_big.bin");

		assert(tifiles_file_is_os(path) == 1);

		/* Only the first page is read for such a file. */
		{
			TiMemReader r;
			assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
			assert(r.size == TIFILES_HEAD_READ_CAP);
			tifiles_mem_reader_destroy(&r);
		}
		free(ns);
	}
}

/* Self-contained: craft minimal files whose headers the classifier must
   recognise, and check the file and buffer APIs agree. */
static void test_synthetic(void)
{
	static const struct
	{
		const char *name;
		const uint8_t *head;
		size_t        head_len;
	}
	synth[] =
	{
		{ "synth.83",  (const uint8_t *)"**TI83**", 8 },
		{ "synth.83g", (const uint8_t *)"**TI83**", 8 },
		{ "synth.tig", (const uint8_t *)"PK\3\4",   4 },
		{ "synth.73",  (const uint8_t *)"**TIFL**", 8 },
	};
	size_t i;

	for (i = 0; i < sizeof(synth) / sizeof(synth[0]); i++)
	{
		char path[5000];
		uint8_t body[96];
		int is_ti, is_single, is_group, is_regular, is_backup, is_os;
		int is_app, is_tib, is_flash, is_tigroup, is_tno;
		int has_ti, has_tib, has_tig, has_tifl, has_tno;
		uint8_t dev_type = 0, data_type = 0;
		TiMemReader r;

		memset(body, 0, sizeof(body));
		memcpy(body, synth[i].head, synth[i].head_len);
		write_file(synth[i].name, body, sizeof(body));
		snprintf(path, sizeof(path), "%s/%s", g_tmpdir, synth[i].name);

		classify(path, &is_ti, &is_single, &is_group, &is_regular,
		         &is_backup, &is_os, &is_app, &is_tib, &is_flash, &is_tigroup, &is_tno);
		has_ti  = tifiles_file_has_ti_header(path);
		has_tib = tifiles_file_has_tib_header(path);
		has_tig = tifiles_file_has_tig_header(path);
		has_tifl = tifiles_file_has_tifl_header(path, &dev_type, &data_type);
		has_tno = tifiles_file_has_tno_header(path);

		assert(is_ti == (has_ti || has_tib || has_tig || has_tifl || has_tno));
		assert(is_tib == has_tib);
		assert(is_tno == has_tno);
		assert(is_tigroup == has_tig);
		assert(is_flash == (has_tib || has_tno || has_tifl));
		assert(is_os == (has_tib || has_tno || (has_tifl && data_type == TI83p_AMS)));
		assert(is_app == (has_tifl && data_type == TI83p_APPL));
		assert(is_single == (is_ti && !is_group && !is_backup && !is_flash && !is_tigroup));

		assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
		assert(tifiles_buffer_is_ti(r.data, r.size) == (has_ti || has_tib || has_tig || has_tifl || has_tno));
		tifiles_mem_reader_destroy(&r);
	}

	{
		char path[5000];
		int is_ti;
		uint8_t body[16];
		memset(body, 'X', sizeof(body));
		write_file("synth_notti.bin", body, sizeof(body));
		snprintf(path, sizeof(path), "%s/%s", g_tmpdir, "synth_notti.bin");
		is_ti = tifiles_file_is_ti(path);
		assert(is_ti == 0);
	}
}

static void test_equivalence(const char *base)
{
	static const char *files[] =
	{
		"ti73/L1L1.73l", "ti73/backup.73b", "ti73/group.73g", "ti73/romdump.73p",
		"ti83/aa.83n", "ti83/backup.83b", "ti83/group.83g", "ti83/squished_ice.83p", "ti83/romdump.83p",
		"ti84p/aa.8Xn", "ti84p/group.8Xg", "ti84p/romdump.8Xp", "ti84p/unsquished.8xp",
		"ti89/group.89g",
		"ti92/backup.92b", "ti92/group2.92g", "ti92/group.92g", "ti92/str.92s",
		"tig/pepe.tig", "tig/test.tig", "tig/test2.tig", "tig/str.89s",
		NULL
	};
	size_t i;

	for (i = 0; files[i] != NULL; i++)
	{
		char path[5200];
		char p0[5200], p1[5200], p2[5200], p3[5200], p4[5200];
		const char *cand[5];
		int k;
		int has_ti, has_tib, has_tig, has_tifl, has_tno;
		int is_ti, is_single, is_group, is_regular, is_backup, is_os;
		int is_app, is_tib, is_flash, is_tigroup, is_tno;
		uint8_t dev_type = 0, data_type = 0;
		const char *e;
		int exp_is_ti, exp_is_tib, exp_is_tno, exp_is_tigroup, exp_is_flash;
		int exp_is_single, exp_is_os, exp_is_app;

		snprintf(p0, sizeof(p0), "%s/%s", base, files[i]);
		snprintf(p1, sizeof(p1), "%s", files[i]);
		snprintf(p2, sizeof(p2), "../%s", files[i]);
		snprintf(p3, sizeof(p3), "../../%s", files[i]);
		snprintf(p4, sizeof(p4), "../../../%s", files[i]);
		cand[0] = p0; cand[1] = p1; cand[2] = p2; cand[3] = p3; cand[4] = p4;

		for (k = 0; k < 5; k++)
		{
			if (access(cand[k], R_OK) == 0)
			{
				break;
			}
		}
		if (k == 5)
		{
			fprintf(stderr, "skipping missing sample %s\n", files[i]);
			continue;
		}
		strncpy(path, cand[k], sizeof(path) - 1);
		path[sizeof(path) - 1] = '\0';

		has_ti  = tifiles_file_has_ti_header(path);
		has_tib = tifiles_file_has_tib_header(path);
		has_tig = tifiles_file_has_tig_header(path);
		has_tifl = tifiles_file_has_tifl_header(path, &dev_type, &data_type);
		has_tno = tifiles_file_has_tno_header(path);

		classify(path, &is_ti, &is_single, &is_group, &is_regular,
		         &is_backup, &is_os, &is_app, &is_tib, &is_flash, &is_tigroup, &is_tno);
		e = tifiles_fext_get(path);

		exp_is_ti = has_ti || has_tib || has_tig || has_tifl || has_tno ||
		            (g_ascii_strcasecmp(e, "tns") == 0);
		exp_is_tib = has_tib;
		exp_is_tno = has_tno;
		exp_is_tigroup = has_tig;
		exp_is_flash = has_tib || has_tno || has_tifl;
		exp_is_os = has_tib || has_tno || (has_tifl && data_type == TI83p_AMS);
		exp_is_app = has_tifl && data_type == TI83p_APPL;
		exp_is_single = exp_is_ti && !is_group && !is_backup && !is_flash && !is_tigroup;

		assert(is_ti == exp_is_ti);
		assert(is_tib == exp_is_tib);
		assert(is_tno == exp_is_tno);
		assert(is_tigroup == exp_is_tigroup);
		assert(is_flash == exp_is_flash);
		assert(is_os == exp_is_os);
		assert(is_app == exp_is_app);
		assert(is_single == exp_is_single);

		if (is_single)
		{
			assert(!is_group);
			assert(!is_backup);
			assert(!is_flash);
			assert(!is_tigroup);
		}

		{
			TiMemReader r;
			assert(tifiles_mem_reader_create_from_file(&r, path, TIFILES_HEAD_READ_CAP) == 0);
			assert(tifiles_buffer_is_ti(r.data, r.size) == (has_ti || has_tib || has_tig || has_tifl || has_tno));
			tifiles_mem_reader_destroy(&r);
		}
	}
}

static void run_memreader_tests(const char *base)
{
	make_tmpdir();
	test_null_and_invalid();
	test_small_and_empty_files();
	test_large_file();
	test_tifl_multi_entry();
	test_synthetic();
	test_equivalence(base);
	printf("memreader tests: OK\n");
}

int main(int argc, char **argv)
{
    void * ptr;

    tifiles_library_init();

    run_memreader_tests(argc > 1 ? argv[1] : ".");

    PRINTF(tifiles_error_get, INT, -1, NULL);
    PRINTF(tifiles_error_free, INT, NULL);
    PRINTF(tifiles_model_to_string, STR, -1);
    PRINTF(tifiles_string_to_model, INT, NULL);
    PRINTF(tifiles_attribute_to_string, STR, -1);
    PRINTF(tifiles_string_to_attribute, INT, NULL);
    PRINTF(tifiles_class_to_string, STR, -1);
    PRINTF(tifiles_string_to_class, INT, NULL);
    PRINTF(tifiles_fext_of_group, STR, -1);
    PRINTF(tifiles_fext_of_backup, STR, -1);

    PRINTF(tifiles_fext_of_flash_app, STR, -1);
    PRINTF(tifiles_fext_of_flash_os, STR, -1);
    PRINTF(tifiles_fext_of_certif, STR, -1);
    PRINTF(tifiles_fext_get, STR, NULL);
    ptr = tifiles_fext_dup(NULL);
    PRINTF(, PTR, ptr);
    tifiles_fext_free(ptr);
    PRINTFVOID(tifiles_fext_free, NULL);
    PRINTF(tifiles_file_is_ti, INT, NULL);
    PRINTF(tifiles_file_is_single, INT, NULL);
    PRINTF(tifiles_file_is_group, INT, NULL);
    PRINTF(tifiles_file_is_regular, INT, NULL);

    PRINTF(tifiles_file_is_backup, INT, NULL);
    PRINTF(tifiles_file_is_os, INT, NULL);
    PRINTF(tifiles_file_is_app, INT, NULL);
    PRINTF(tifiles_file_is_tib, INT, NULL);
    PRINTF(tifiles_file_is_flash, INT, NULL);
    PRINTF(tifiles_file_is_tigroup, INT, NULL);
    PRINTF(tifiles_file_is_tno, INT, NULL);
    PRINTF(tifiles_file_has_ti_header, INT, NULL);
    PRINTF(tifiles_file_has_tib_header, INT, NULL);
    PRINTF(tifiles_file_has_tig_header, INT, NULL);

    PRINTF(tifiles_file_has_tifl_header, INT, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(tifiles_file_has_tno_header, INT, NULL);
    PRINTF(tifiles_model_to_dev_type, INT, -1);
    PRINTF(tifiles_file_test, INT, NULL, -1, -1);
    PRINTF(tifiles_file_get_model, INT, NULL);
    PRINTF(tifiles_file_get_class, INT, NULL);
    PRINTF(tifiles_file_get_type, STR, NULL);
    PRINTF(tifiles_file_get_icon, STR, NULL);
    PRINTF(tifiles_vartype2string, STR, -1, -1);
    PRINTF(tifiles_string2vartype, INT, -1, NULL);

    PRINTF(tifiles_vartype2fext, STR, -1, -1);
    PRINTF(tifiles_fext2vartype, INT, -1, NULL);
    PRINTF(tifiles_vartype2type, STR, -1, -1);
    PRINTF(tifiles_vartype2icon, STR, -1, -1);
    PRINTF(tifiles_calctype2signature, STR, -1);
    PRINTF(tifiles_signature2calctype, INT, NULL);
    PRINTF(tifiles_folder_type, INT, -1);
    PRINTF(tifiles_flash_type, INT, -1);
    PRINTF(tifiles_idlist_type, INT, -1);
    PRINTF(tifiles_calc_is_ti8x, INT, -1);

    PRINTF(tifiles_calc_is_ti9x, INT, -1);
    PRINTF(tifiles_calc_are_compat, INT, -1, -1);
    PRINTF(tifiles_has_folder, INT, -1);
    PRINTF(tifiles_is_flash, INT, -1);
    PRINTF(tifiles_has_backup, INT, -1);
    PRINTF(tifiles_has_attrs, INT, -1);
    PRINTF(tifiles_checksum, INT, NULL, 1234567891);
    PRINTF(tifiles_hexdump, INT, NULL, 1);
    PRINTF(tifiles_get_varname, STR, NULL);
    PRINTF(tifiles_get_fldname, STR, NULL);

    PRINTF(tifiles_get_fldname_s, STR, NULL, (void *)0x12345678);
    PRINTF(tifiles_get_fldname_s, STR, (void *)0x12345678, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, NULL, (void *)0x12345678);
    PRINTF(tifiles_build_fullname, STR, -1, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_build_fullname, STR, -1, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_build_filename, STR, -1, NULL);
    PRINTFVOID(tifiles_filename_free, NULL);
    ptr = tifiles_content_create_regular(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_regular(ptr);
    PRINTF(tifiles_content_delete_regular, INT, NULL);
    PRINTF(tifiles_file_read_regular, INT, NULL, (void *)0x12345678);

    PRINTF(tifiles_file_read_regular, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_regular, INT, NULL, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_regular, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_regular, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_file_display_regular, INT, NULL);
    ptr = tifiles_content_create_backup(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_backup(ptr);
    PRINTF(tifiles_content_delete_backup, INT, NULL);
    PRINTF(tifiles_file_read_backup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_read_backup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_backup, INT, NULL, (void *)0x12345678);

    PRINTF(tifiles_file_write_backup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_display_backup, INT, NULL);
    ptr = tifiles_content_create_flash(-1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_flash(ptr);
    PRINTF(tifiles_file_read_flash, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_content_delete_flash, INT, NULL);
    PRINTF(tifiles_file_read_flash, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_flash, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash2, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_flash2, INT, (void *)0x12345678, NULL, NULL);

    PRINTF(tifiles_file_display_flash, INT, NULL);
    PRINTF(tifiles_content_dup_regular, PTR, NULL);
    PRINTF(tifiles_content_dup_flash, PTR, NULL);
    PRINTF(tifiles_file_display, INT, NULL);
    PRINTF(tifiles_file_get_metadata, PTR, NULL);
    PRINTFVOID(tifiles_metadata_free, NULL);
    PRINTF(tifiles_metadata_add_pair, INT, NULL, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678);
    PRINTF(tifiles_metadata_add_pair, INT, (void *)0x12345678, NULL, (void *)0x12345678, (void *)0x12345678);
    PRINTF(tifiles_metadata_add_pair, INT, (void *)0x12345678, (void *)0x12345678, NULL, (void *)0x12345678);
    PRINTF(tifiles_metadata_add_pair, INT, (void *)0x12345678, (void *)0x12345678, (void *)0x12345678, NULL);

    PRINTF(tifiles_metadata_find_pair, PTR, NULL, (void *)0x12345678);
    PRINTF(tifiles_metadata_find_pair, PTR, (void *)0x12345678, NULL);
    ptr = tifiles_content_create_group(0);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_group(ptr);
    PRINTF(tifiles_content_delete_group, INT, NULL);
    PRINTF(tifiles_group_contents, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_contents, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_ungroup_content, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_ungroup_content, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_group_files, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_files, INT, (void *)0x12345678, NULL);

    PRINTF(tifiles_ungroup_file, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_content_add_entry, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_content_del_entry, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_add_file, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_add_file, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_group_del_file, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_group_del_file, INT, (void *)0x12345678, NULL);
    ptr = tifiles_content_create_tigroup(-1, -1);
    PRINTF(, PTR, ptr);
    tifiles_content_delete_tigroup(ptr);
    PRINTF(tifiles_content_delete_tigroup, INT, NULL);
    PRINTF(tifiles_file_read_tigroup, INT, NULL, (void *)0x12345678);

    PRINTF(tifiles_file_read_tigroup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_write_tigroup, INT, NULL, (void *)0x12345678);
    PRINTF(tifiles_file_write_tigroup, INT, (void *)0x12345678, NULL);
    PRINTF(tifiles_file_display_tigroup, INT, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, (void *)0x12345678, NULL);
    PRINTF(tifiles_tigroup_contents, INT, (void *)0x12345678, NULL, NULL);
    PRINTF(tifiles_tigroup_contents, INT, NULL, NULL, NULL);
    PRINTF(tifiles_untigroup_content, INT, NULL, NULL, NULL);
    PRINTF(tifiles_untigroup_content, INT, NULL, NULL, NULL);
    PRINTF(tifiles_untigroup_content, INT, NULL, NULL, NULL);

    PRINTF(tifiles_tigroup_files, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_files, INT, NULL, NULL);
    PRINTF(tifiles_untigroup_file, INT, NULL, NULL);
    PRINTF(tifiles_untigroup_file, INT, NULL, NULL);
    PRINTF(tifiles_content_add_te, INT, NULL, NULL);
    PRINTF(tifiles_content_add_te, INT, NULL, NULL);
    PRINTF(tifiles_content_del_te, INT, NULL, NULL);
    PRINTF(tifiles_content_del_te, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_add_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_add_file, INT, NULL, NULL);

    PRINTF(tifiles_tigroup_del_file, INT, NULL, NULL);
    PRINTF(tifiles_tigroup_del_file, INT, NULL, NULL);
    PRINTF(tifiles_te_create, PTR, NULL, -1, -1);
    PRINTF(tifiles_te_delete, INT, NULL);
    ptr = tifiles_te_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_te_delete_array(ptr);
    ptr = tifiles_te_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_te_delete_array(ptr);
    PRINTFVOID(tifiles_te_delete_array, NULL);
    PRINTF(tifiles_te_sizeof_array, INT, NULL);
    PRINTF(tifiles_comment_set_single, STR);
    PRINTF(tifiles_comment_set_group, STR);

    PRINTF(tifiles_comment_set_backup, STR);
    PRINTF(tifiles_comment_set_tigroup, STR);
    PRINTF(tifiles_comment_set_single_sn, STR, NULL, 1234567891);
    PRINTF(tifiles_comment_set_group_sn, STR, NULL, 1234567891);
    PRINTF(tifiles_comment_set_backup_sn, STR, NULL, 1234567891);
    PRINTF(tifiles_comment_set_tigroup_sn, STR, NULL, 1234567891);
    ptr = tifiles_ve_create();
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_with_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);
    ptr = tifiles_ve_create_with_data2(0, NULL);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete(ptr);

    PRINTFVOID(tifiles_ve_delete, NULL);
    ptr = tifiles_ve_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_free_data(ptr);
    ptr = tifiles_ve_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    tifiles_ve_free_data(ptr);
    PRINTFVOID(tifiles_ve_free_data, NULL);
    PRINTF(tifiles_ve_copy, PTR, NULL, NULL);
    PRINTF(tifiles_ve_dup, PTR, NULL);
    ptr = tifiles_ve_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete_array(ptr);
    ptr = tifiles_ve_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_ve_delete_array(ptr);
    PRINTFVOID(tifiles_ve_delete_array, NULL);
    ptr = tifiles_fp_create();
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);

    ptr = tifiles_fp_create_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    ptr = tifiles_fp_create_with_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    ptr = tifiles_fp_create_with_data2(0, NULL);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete(ptr);
    PRINTFVOID(tifiles_fp_delete, NULL);
    ptr = tifiles_fp_alloc_data(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_free_data(ptr);
    ptr = tifiles_fp_realloc_data(NULL, 1);
    PRINTF(, PTR, ptr);
    tifiles_fp_free_data(ptr);
    PRINTFVOID(tifiles_fp_free_data, NULL);
    ptr = tifiles_fp_create_array(0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete_array(ptr);
    ptr = tifiles_fp_resize_array(NULL, 0);
    PRINTF(, PTR, ptr);
    tifiles_fp_delete_array(ptr);
    PRINTFVOID(tifiles_fp_delete_array, NULL);
    ptr = tifiles_create_table_of_entries(NULL, NULL);

    PRINTF(, PTR, ptr);
    tifiles_free_table_of_entries(ptr);
    PRINTFVOID(tifiles_free_table_of_entries, NULL);

    cert_functions_unit_test();

    tifiles_library_exit();

    return 0;
}
