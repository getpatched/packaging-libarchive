#include <archive.h>
#include <archive_entry.h>

#include <stdint.h>
#include <stdio.h>

static la_ssize_t
write_callback(struct archive *a, void *client_data, const void *buffer, size_t length)
{
	(void)a;
	(void)client_data;
	(void)buffer;
	/* The regression only exercises ZIP header generation. */
	return (la_ssize_t)length;
}

static int
close_callback(struct archive *a, void *client_data)
{
	(void)a;
	(void)client_data;
	return (ARCHIVE_OK);
}

int
main(void)
{
	struct archive *a;
	struct archive_entry *entry;
	int r;

	a = archive_write_new();
	if (a == NULL)
		return (2);
	archive_write_set_format_zip(a);
	archive_write_add_filter_none(a);
	r = archive_write_set_format_option(a, "zip", "encryption", "aes256");
	if (r != ARCHIVE_OK) {
		fprintf(stderr, "AES-256 ZIP option unavailable: %s\n",
		    archive_error_string(a));
		archive_write_free(a);
		return (2);
	}
	archive_write_set_passphrase(a, "secret");
	r = archive_write_open(a, NULL, NULL, write_callback, close_callback);
	if (r != ARCHIVE_OK) {
		fprintf(stderr, "archive_write_open failed: %s\n",
		    archive_error_string(a));
		archive_write_free(a);
		return (2);
	}

	entry = archive_entry_new();
	if (entry == NULL) {
		archive_write_free(a);
		return (2);
	}
	archive_entry_set_pathname(entry, "poc");
	archive_entry_set_filetype(entry, AE_IFREG);
	archive_entry_set_perm(entry, 0644);
	archive_entry_set_size(entry, INT64_MAX);
	fputs("GETPATCHED:ZIP_HEADER_START\n", stderr);
	fflush(stderr);
	r = archive_write_header(a, entry);
	fputs("GETPATCHED:ZIP_HEADER_RETURN\n", stderr);
	fflush(stderr);
	if (r != ARCHIVE_OK)
		fprintf(stderr, "archive_write_header failed: %s\n",
		    archive_error_string(a));
	archive_entry_free(entry);
	archive_write_free(a);
	return (r == ARCHIVE_OK ? 0 : 1);
}
