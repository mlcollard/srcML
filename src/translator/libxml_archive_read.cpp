/*
  libxml_archive_read.cpp

  Copyright (C) 2010-2014  SDML (www.srcML.org)

  This file is part of the srcML Toolkit.

  The srcML Toolkit is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  The srcML Toolkit is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the srcML Toolkit; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "libxml_archive_read.hpp"
#include <stdio.h>
#include <string.h>
#include <libxml/xmlIO.h>
#include <fnmatch.h>
#include <archive.h>
#include <archive_entry.h>
#include <string>
#include <libxml/nanohttp.h>
#include <libxml/nanoftp.h>
#include <vector>
#include <regex.h>

static const char* ARCHIVE_FILTER_EXTENSIONS[] = {".tar", ".zip", ".tgz", ".cpio", ".shar", ".gz", ".bz2", ".xz", 0};

struct archiveData {
    struct archive* a;
    struct archive_entry* ae;
    int status;
    std::string root_filename;
    void* libxmlcontext;
    bool ishttp;
};

archiveData* current_context = 0;

bool archiveIsDir(void* context) {

    archiveData* pcontext = (archiveData*) context;

    return pcontext && pcontext->ae && archive_entry_filetype(pcontext->ae) == AE_IFDIR;
}

bool isAnythingOpen(void* context) {

    archiveData* pcontext = (archiveData*) context;

    return pcontext && pcontext->status != ARCHIVE_EOF;
}

// check if file has an archive extension
bool isArchiveRead(void* context) {

    archiveData* pcontext = (archiveData*) context;

    return pcontext && pcontext->a && pcontext->status == ARCHIVE_OK
#if ARCHIVE_VERSION_NUMBER >= 2008000
        && (archive_format(pcontext->a) != ARCHIVE_FORMAT_RAW)
#else
        && (archive_format(pcontext->a) != ARCHIVE_FORMAT_EMPTY)
#endif
        ;
}

// format (e.g., tar, cpio) of the current file
const char* archiveReadFormat(void* context) {

    archiveData* pcontext = (archiveData*) context;

    return (!pcontext || !pcontext->a) ? 0 : archive_format_name(pcontext->a);
}

// compression (e.g., gz, bzip2) of the current file
const char* archiveReadCompression(void* context) {

    archiveData* pcontext = (archiveData*) context;
#if ARCHIVE_VERSION_NUMBER <= 3000200
    return (!pcontext || !pcontext->a) ? 0 : archive_compression_name(pcontext->a);
#else
    return (!pcontext || !pcontext->a) ? 0 : archive_filter_name(pcontext->a, 0);
#endif
}

// match the extension
int archiveReadMatchExtension(const char* URI) {

    const char* tails[] = {".tar.gz", ".zip.gz", ".cpio.gz" };

    // allow libxml to handle non-archive files encrypted with gz
    if (strcmp(URI + strlen(URI) - strlen(".gz"), ".gz") == 0) {

        for(unsigned int i = 0; i < sizeof(tails) / sizeof(tails[0]); ++i)

            if (strcmp(URI + strlen(URI) - strlen(tails[i]), tails[i]) == 0)
                break;

        return 0;

    }

    if ((URI[0] == '-' && URI[1] == '\0') || (strcmp(URI, "/dev/stdin") == 0))
        return 0;

    for(const char** pos = ARCHIVE_FILTER_EXTENSIONS;*pos != 0; ++pos )
    {
        /*      char pattern[10] = { 0 } ;
                strcpy(pattern, "*.");
                strcat(pattern, *pos);
        */
        if(int match = strcmp(URI + strlen(URI) - strlen(*pos), *pos) == 0)
            return match;
    }

    return 0;
}

// check if archive matches the protocol on the URI
int archiveReadMatch(const char* URI) {

    if (URI == NULL)
        return 0;

    // when the xslt option is used, the input cannot come through libarchive/libxml
    // so fix here so that it is not used
    if (strcmp(URI + strlen(URI) - strlen(".xsl"), ".xsl") == 0)
        return 0;

#if ARCHIVE_VERSION_NUMBER >= 2008000
    // put all input through libarchive for automatic detection of the format
    return 1;

#else
    // match archive extensions
    return archiveReadMatchExtension(URI);
#endif
}

// setup archive root for this URI
int archiveReadStatus(void* context) {

    archiveData* pcontext = (archiveData*) context;

    return pcontext->status;
}

const char* archiveReadFilename(void* context) {

    archiveData* pcontext = (archiveData*) context;

    if (!pcontext || !pcontext->ae ||
        (archiveReadStatus(context) != ARCHIVE_OK && archiveReadStatus(context) != ARCHIVE_EOF))
        return 0;

    return isArchiveRead(context) ? archive_entry_pathname(pcontext->ae) : 0;
}

static int archive_read_open_http_callback(struct archive* a,
                                           void* _client_data) {

    archiveData* pcontext = (archiveData*) _client_data;

    pcontext->libxmlcontext = (archiveData*) (pcontext->ishttp ? xmlNanoHTTPOpen(pcontext->root_filename.c_str(), 0) : xmlNanoFTPOpen(pcontext->root_filename.c_str()));

    return 0;
}

static
#if ARCHIVE_VERSION_NUMBER >= 2008000
__LA_SSIZE_T
#else
ssize_t
#endif
archive_read_http_callback(struct archive* a,
                           void* _client_data, const void** _buffer) {

    archiveData* pcontext = (archiveData*) _client_data;

    static const int len = 4096;
    static std::vector<char> data(len);
    *_buffer = &data[0];
    int size = pcontext->ishttp ? xmlNanoHTTPRead(pcontext->libxmlcontext, &data[0], len) : xmlNanoFTPRead(pcontext->libxmlcontext, &data[0], len);

    return size;
}

static int archive_read_close_http_callback(struct archive* a,
                                            void* _client_data) {

    archiveData* pcontext = (archiveData*) _client_data;

    if (pcontext->ishttp)
        xmlNanoHTTPClose(pcontext->libxmlcontext);
    else
        xmlNanoFTPClose(pcontext->libxmlcontext);

    return 1;
}

// setup archive for this URI
void* archiveReadOpen(const char* URI) {

    if (!archiveReadMatch(URI))
        return NULL;

    // current_context is explicitly set whenever an archive is a possibility
    if (current_context) {
        archiveData* context = current_context;
        current_context = 0;
        return context;
    }

    archiveData* gpcontext = 0;
    gpcontext = new archiveData;
    gpcontext->status = 0;
    gpcontext->a = archive_read_new();
    //archive_read_support_compression_all(gpcontext->a);
#if ARCHIVE_VERSION_NUMBER >= 3000003
#if ARCHIVE_VERSION_NUMBER <= 3000200
    archive_read_support_compression_bzip2(gpcontext->a);
    archive_read_support_compression_gzip(gpcontext->a);
#else
    archive_read_support_filter_bzip2(gpcontext->a);
    archive_read_support_filter_gzip(gpcontext->a);
#endif
#else
    archive_read_support_compression_all(gpcontext->a);
#endif

#if ARCHIVE_VERSION_NUMBER >= 3001002
    archive_read_support_filter_xz(gpcontext->a);
#endif

#if ARCHIVE_VERSION_NUMBER >= 2008000
    archive_read_support_format_raw(gpcontext->a);
#endif

#if ARCHIVE_VERSION_NUMBER >= 3000003
    archive_read_support_format_empty(gpcontext->a);
    archive_read_support_format_tar(gpcontext->a);
    archive_read_support_format_zip(gpcontext->a);
    archive_read_support_format_cpio(gpcontext->a);
#else
    archive_read_support_format_all(gpcontext->a);
#endif

    //    int r = archive_read_open_filename(a, URI, 4000);
    gpcontext->ishttp = xmlIOHTTPMatch(URI);
    if (gpcontext->ishttp || xmlIOFTPMatch(URI)) {
        gpcontext->root_filename = URI;
        gpcontext->status = archive_read_open(gpcontext->a, gpcontext, archive_read_open_http_callback, archive_read_http_callback,
                                              archive_read_close_http_callback);
    } else {
        gpcontext->status = archive_read_open_filename(gpcontext->a, strcmp(URI, "-") == 0 ? 0 : URI, 4000);
    }
    if (gpcontext->status != ARCHIVE_OK) {
#if ARCHIVE_VERSION_NUMBER <= 3000200
        archive_read_finish(gpcontext->a);
#else
        archive_read_free(gpcontext->a);
#endif
        delete gpcontext;
        return 0;
    }

    gpcontext->status = archive_read_next_header(gpcontext->a, &gpcontext->ae);
    if (gpcontext->status != ARCHIVE_EOF && gpcontext->status != ARCHIVE_OK) {
#if ARCHIVE_VERSION_NUMBER <= 3000200
        archive_read_finish(gpcontext->a);
#else
        archive_read_free(gpcontext->a);
#endif
        gpcontext->a = 0;
        return 0;
    }

    return gpcontext;
}

// close the open file
int archiveReadClose(void* context) {

    archiveData* pcontext = (archiveData*) context;

    //  if (context == NULL)
    //    return -1;

    if (pcontext->status != ARCHIVE_OK)
        return 0;

    // read the next header.  If there isn't one, then really finish
    pcontext->status = archive_read_next_header(pcontext->a, &pcontext->ae);
    if (pcontext->status != ARCHIVE_OK)
#if ARCHIVE_VERSION_NUMBER <= 3000200
        archive_read_finish(pcontext->a);
#else
        archive_read_free(pcontext->a);
#endif

    return 0;
}

// read from the URI
int archiveRead(void* context, char* buffer, int len) {

    archiveData* pcontext = (archiveData*) context;

    if (pcontext->status != ARCHIVE_OK)
        return 0;

    size_t size = archive_read_data(pcontext->a, buffer, len);
    if ((signed)size < 0)
        return 0;

    return size;
}

void archiveDeleteContext(void* context) {

    archiveData* pcontext = (archiveData*) context;

    delete pcontext;
}
