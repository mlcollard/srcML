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

# set icon
install(FILES ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico DESTINATION . COMPONENT SRCML)
set(CPACK_WIX_PRODUCT_ICON ${CMAKE_SOURCE_DIR}/package/srcml_icon.ico)

# already set
set(CPACK_NSIS_MODIFY_PATH ON)

# directory name
set(CPACK_PACKAGE_INSTALL_DIRECTORY "srcML")

# set contact in add/remove programs
set(CPACK_NSIS_CONTACT "srcML <srcML.org>")

# Custom GUID
set(CPACK_WIX_PRODUCT_GUID "8EB65BD8-DF5B-4DED-A642-80BDBBDEB58C")

# Custom GUID
set(CPACK_WIX_UPGRADE_GUID "278604DF-BD50-410A-9475-993006D98663")

# Background on dialogs
set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}/background.png")

# ID of custom UI
set(CPACK_WIX_UI_REF "srcMLUI_InstallDir")

# Add to CMake registry so that other CMake files can use find_program()
set(CPACK_WIX_CMAKE_PACKAGE_REGISTRY srcML)

# Extra dialog for adding to path
set(CPACK_WIX_EXTRA_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/srcml_extra_dialog.wxs" "${CMAKE_CURRENT_SOURCE_DIR}/install_dir.wxs")

# Path for path environment
set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_SOURCE_DIR}/patch_path_env.xml")

# Template instead of default WiX template
set(CPACK_WIX_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/WIX.template.in")
