##
# @file nsis.cmake
#
# @copyright Copyright (C) 2014 srcML, LLC. (www.srcML.org)
# 
# The srcML Toolkit is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# The srcML Toolkit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with the srcML Toolkit; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# set icons
set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico)
set(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico)
set(CPACK_NSIS_INSTALLED_ICON_NAME srcml_icon.ico)

set(CPACK_NSIS_MODIFY_PATH ON)
if(WIN32)
    set(MSVC_REDIST ${WINDOWS_DEP_PATH}/tools/VC_redist.${BUILD_ARCH}.exe)
    get_filename_component(vcredist_name "${MSVC_REDIST}" NAME)
    install(PROGRAMS ${MSVC_REDIST} DESTINATION bin)
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "ExecWait '\\\"$INSTDIR\\\\bin\\\\${vcredist_name}\\\" /passive /norestart'")
endif()
# set contact in add/remove programs
set(CPACK_NSIS_CONTACT "Software Developement Laboratories <bugs@srcML.org>")
