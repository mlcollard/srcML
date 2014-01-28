/*
  test_srcml_write_open.cpp

  Copyright (C) 2013-2014  SDML (www.srcML.org)

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

/*

  Test cases for write open for archives
*/
#include <stdio.h>
#include <string.h>
#include <cassert>
#include <fstream>
#if defined(__GNUC__) && !defined(__MINGW32__)
#include <unistd.h>
#endif
#include <fcntl.h>

#include <srcml.h>
#include <srcml_types.hpp>
#include <srcmlns.hpp>

#include "dassert.hpp"

int main() {

  /* 
     srcml_write_open_filename
  */

  {

  srcml_archive * archive = srcml_create_archive();
  srcml_write_open_filename(archive, "project.xml");

  dassert(archive->type, SRCML_ARCHIVE_WRITE);
  dassert(!archive->translator, 0);
  dassert(srcml_archive_get_options(archive), (SRCML_OPTION_ARCHIVE | SRCML_OPTION_XML_DECL | SRCML_OPTION_NAMESPACE_DECL));

  srcml_close_archive(archive);
  srcml_free_archive(archive);

  // check file either does not exist or empty

  }

  {

  srcml_archive * archive = srcml_create_archive();
  dassert(srcml_write_open_filename(archive, 0), SRCML_STATUS_ERROR);

  srcml_free_archive(archive);

  }

  {
  dassert(srcml_write_open_filename(0, "project.xml"), SRCML_STATUS_ERROR);
  }

  /* 
     srcml_write_open_memory
  */

  {

  char * s = 0;
  int size;
  srcml_archive * archive = srcml_create_archive();
  srcml_write_open_memory(archive, &s, &size);

  dassert(archive->type, SRCML_ARCHIVE_WRITE);
  dassert(!archive->translator, 0);
  dassert(srcml_archive_get_options(archive), (SRCML_OPTION_ARCHIVE | SRCML_OPTION_XML_DECL | SRCML_OPTION_NAMESPACE_DECL));

  srcml_close_archive(archive);
  srcml_free_archive(archive);
  free(s);

  dassert(strlen(s), 0);
  }

  {

  int size;
  srcml_archive * archive = srcml_create_archive();
  dassert(srcml_write_open_memory(archive, 0, &size), SRCML_STATUS_ERROR);

  srcml_free_archive(archive);
  }

  {

  char * s = 0;
  srcml_archive * archive = srcml_create_archive();
  dassert(srcml_write_open_memory(archive, &s, 0), SRCML_STATUS_ERROR);

  srcml_free_archive(archive);
  free(s);
  }

  {
  char * s = 0;
  int size;
  dassert(srcml_write_open_memory(0, &s, &size), SRCML_STATUS_ERROR);
  free(s);
  }

  /* 
     srcml_write_open_FILE
  */

  {
  FILE * file = fopen("project.xml", "w");

  srcml_archive * archive = srcml_create_archive();
  srcml_write_open_FILE(archive, file);

  dassert(archive->type, SRCML_ARCHIVE_WRITE);
  dassert(!archive->translator, 0);
  dassert(srcml_archive_get_options(archive), (SRCML_OPTION_ARCHIVE | SRCML_OPTION_XML_DECL | SRCML_OPTION_NAMESPACE_DECL));

  srcml_close_archive(archive);
  srcml_free_archive(archive);
  fclose(file);

  char buf[1];
  file = fopen("project.xml", "r");
  size_t num_read = fread(buf, 1, 1, file);
  fclose(file);
  dassert(num_read, 0);

  }

  {

  srcml_archive * archive = srcml_create_archive();
  dassert(srcml_write_open_FILE(archive, 0), SRCML_STATUS_ERROR);

  srcml_free_archive(archive);

  }

  {
  FILE * file = fopen("project_ns.xml", "w");
  dassert(srcml_write_open_FILE(0, file), SRCML_STATUS_ERROR);
  fclose(file);
  }

  /* 
     srcml_write_open_fd
  */

  {
  int fd = open("project.xml", O_WRONLY);

  srcml_archive * archive = srcml_create_archive();
  srcml_write_open_fd(archive, fd);

  dassert(archive->type, SRCML_ARCHIVE_WRITE);
  dassert(!archive->translator, 0);
  dassert(srcml_archive_get_options(archive), (SRCML_OPTION_ARCHIVE | SRCML_OPTION_XML_DECL | SRCML_OPTION_NAMESPACE_DECL));

  srcml_close_archive(archive);
  srcml_free_archive(archive);
  close(fd);

  char buf[1];
  fd = open("project.xml", O_RDONLY);
  size_t num_read = read(fd, buf, 1);
  close(fd);
  dassert(num_read, 0);
  }

  {

  srcml_archive * archive = srcml_create_archive();
  dassert(srcml_write_open_fd(archive, -1), SRCML_STATUS_ERROR);

  srcml_free_archive(archive);

  }

  {
  int fd = open("project_ns.xml", O_WRONLY);
  dassert(srcml_write_open_fd(0, fd), SRCML_STATUS_ERROR);
  close(fd);
  }

  unlink("project.xml");
  unlink("project_ns.xml");

  srcml_cleanup_globals();

  return 0;

}
