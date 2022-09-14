##
# @file nsis.cmake
#
# @copyright Copyright (C) 2014-2019 srcML, LLC. (www.srcML.org)
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

# set icons
install(FILES ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico DESTINATION . COMPONENT SRCML)
set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico)
set(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico)
set(CPACK_NSIS_INSTALLED_ICON_NAME srcml_icon.ico)

# already set
set(CPACK_NSIS_MODIFY_PATH ON)

# directory name
set(CPACK_PACKAGE_INSTALL_DIRECTORY "srcML")

# set contact in add/remove programs
set(CPACK_NSIS_CONTACT "srcML <srcML.org>")

# which components are disabled
# set(CPACK_COMPONENT_SRCML_DISABLED False)
# set(CPACK_COMPONENT_DEVLIBS_DISABLED False)
# set(CPACK_COMPONENT_SRCMLDEV_DISABLED True)
# set(CPACK_COMPONENT_EXAMPLES_DISABLED True)

set(CPACK_WIX_EXTRA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/srcml_extra_dialog.wxs")

set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/patch_path_env.xml")

set(CPACK_WIX_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/WIX.template.in")
