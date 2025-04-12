##
# docker-bake.hcl
#
# Defines how the srcml Linux distribution Docker images are built. It builds images to:
#
# * Provide a build environment
# * Create and package installers
# * Test the installers
#
# The default is to create all build environment docker images:
#
#   docker buildx bake
#
# To see the full list of available targets, including distribution-specific ones:
#
#   docker buildx bake --list targets
#
# To see the available variables:
#
#   docker buildx bake --list variables
#
# In addition to the default build environment, there are specific targets for the following:
#
# * `build` - Build srcML and create the package installer
# * `image` - Create an image with only the installer files (no o.s.)
# * `files` - Download the installer files to a host directory
# * `logs`  - Download the test logs to a host directory

# Override using the environment variable BAKE_SRC. E.g.,
#   BAKE_SRC="."
#   BAKE_SRC="srcml.tar.gz"
#   BAKE_SRC="https://github.com/srcML/srcML.git"
#   BAKE_SRC="https://github.com/srcML/srcML.git#develop"
variable "BAKE_SRC" {
  # description = "Location of the source code"
  default = "."
}

# Override using the environment variable BAKE_PRESET_SUFFIX
# E.g., BAKE_PRESET_SUFFIX="-fast"
variable "BAKE_PRESET_SUFFIX" {
  # description = "Suffix for the workflow preset"
  default = ""
}

# Override using the environment variable BAKE_ARCHITECTURES
# E.g., BAKE_ARCHITECTURES="linux/arm64"
variable "BAKE_ARCHITECTURES" {
  # description = "Architectures to build on"
  default = "linux/amd64,linux/arm64"
}

# Override using the environment variable BAKE_DESTINATION_DIR
# E.g., BAKE_DESTINATION_DIR="../dists"
variable "BAKE_DESTINATION_DIR" {
  # description = "Local directory for export of packages"
  default = "./dist_packages"
}

# Override using the environment variable BAKE_CONTEXT_DIR
# E.g., BAKE_CONTEXT_DIR="~/srcML/docker"
variable "BAKE_CONTEXT_DIR" {
  # description = "Directory of context files"
  default = "./docker"
}

# Override using the environment variable BAKE_REGISTRY. E.g.,
#   BAKE_REGISTRY=""         docker buildx bake # Docker Hub
#   BAKE_REGISTRY="ghcr.io"  docker buildx bake # GitHub Container Registry
variable "BAKE_REGISTRY" {
  # description = "Registry domain for default build environments"
  default = ""
}

# Override using the environment variable BAKE_PACKAGE_REGISTRY. E.g.,
#   BAKE_PACKAGE_REGISTRY=""         docker buildx bake # Docker Hub
#   BAKE_PACKAGE_REGISTRY="ghcr.io"  docker buildx bake # GitHub Container Registry
variable "BAKE_PACKAGE_REGISTRY" {
  # description = "Registry domain for the package targets, package and log"
  default = "ghcr.io"
}

# Placeholder to redefine in docker-bake.override.hcl
variable "BAKE_SRCML_VERSION" {
  # description = "srcML version to embed in image data"
  default = ""
}

# Placeholder to redefine in docker-bake.override.hcl
variable "BAKE_CMAKE_VERSION" {
  # description = "CMake version"
  default = ""
}

# Placeholder for distributions. See docker-bake.override.hcl for configured
# Linux distributions and distribution-specific groups and targets
variable "distributions" {
  default = [ { id = "ubuntu", version_id = "24.04", name = "Ubuntu 24.04", workflow = "ubuntu", java = "latest" } ]
}

# Default is to only create the build environment
group "default" {
  targets = [ for item in distributions : item.id ]
}

# Context of the Dockerfile
function "context" {
  params = [id]
  result = "${BAKE_CONTEXT_DIR}/${id}"
}

# Context of the Dockerfile
function "workflowPreset" {
  params = [dist]
  result = "ci-${dist.workflow}${BAKE_PRESET_SUFFIX}"
}

# Base target for common settings
target "base" {
  platforms = split(",", BAKE_ARCHITECTURES)
  labels = {
    "org.opencontainers.image.version"  = BAKE_SRCML_VERSION
    "org.opencontainers.image.licenses" = "GPL-3.0-only"
  }
  pull = true
}

# Build images for all Linux distributions
target "default" {
  name = targetName(dist)
  description = "srcML build environment for ${dist.name}"
  labels = {
    "org.opencontainers.image.title" = "srcML ${dist.name} Build Environment"
    "org.opencontainers.image.description" = <<EOF
The srcML build environment for ${dist.name}.
It includes the dependencies for building, packaging, and testing srcML on this platform.
EOF
  }
  matrix = {
    dist = distributions
  }
  context = context(dist.id)
  args = {
    TAG          = dist.version_id,
    CMAKE_BINARY = try(dist.cmake, "")
    CMAKE_VERSION = BAKE_CMAKE_VERSION
    JAVA_VERSION = try(dist.java, "")
    OPENSUSE     = try(dist.opensuse, "")
  }
  tags     = [tagName(dist)]
  inherits = ["base"]
}

# Dockerfile to build the package
function "builderStage" {
  params = [dist]
  result = <<EOF
FROM ${tagName(dist)} AS builder
WORKDIR /src
ADD --link ["${BAKE_SRC}", "."]
RUN cmake --workflow --preset ${workflowPreset(dist)}
EOF
}

# Packages for all distributions
# Output to host directory ${BAKE_DESTINATION_DIR}
# Create image whose only contents are the package
target "build" {
  name = categoryTarget(dist, "build")
  description = "srcML build for ${dist.name}"
  labels = {
    "org.opencontainers.image.title" = "srcML ${dist.name} Build"
    "org.opencontainers.image.description" = <<EOF
The srcML build and package for ${dist.name}.
EOF
    "org.opencontainers.image.source" = BAKE_SRC
  }
  push = true
  matrix = {
    dist = distributions
  }
  dockerfile-inline = builderStage(dist)
  tags     = [categoryTagName(dist, "build")]
  # output   = ["type=docker"]
  inherits = ["base"]
}

# Extract the installers
function "installerStage" {
  params = []
  result = <<EOF
FROM scratch AS dist
COPY --from="builder" \
  /src-build/dist/*.rpm \
  /src-build/dist/*.deb \
  /src-build/dist/*.tar.gz \
  /src-build/dist/*.bz2 \
  /
EOF
}

# Packages for all distributions
# Output to host directory ${BAKE_DESTINATION_DIR}
# Create image whose only contents are the package
target "files" {
  name = categoryTarget(dist, "files")
  description = "srcML package for ${dist.name}"
  labels = {
    "org.opencontainers.image.title" = "srcML ${dist.name} Package Files"
    "org.opencontainers.image.description" = <<EOF
The srcML package files for ${dist.name}.
EOF
  }
  matrix = {
    dist = distributions
  }
  dockerfile-inline = <<EOF
${builderStage(dist)}
${installerStage()}
EOF
  tags     = [categoryTagName(dist, "files")]
  output   = ["type=local,dest=${BAKE_DESTINATION_DIR}"]
  inherits = ["base"]
}

target "image" {
  name = categoryTarget(dist, "image")
  description = "srcML package image for ${dist.name}"
  labels = {
    "org.opencontainers.image.title" = "srcML ${dist.name} Package Image"
    "org.opencontainers.image.description" = <<EOF
The srcML package image for ${dist.name}.
EOF
  }
  matrix = {
    dist = distributions
  }
  dockerfile-inline = <<EOF
${builderStage(dist)}
${installerStage()}
EOF
  tags     = [categoryTagName(dist, "image")]
  # output   = ["type=registry"]
  inherits = ["base"]
}

# Test logs for all distributions
# Output to host directory ${BAKE_DESTINATION_DIR}
target "logs" {
  name = categoryTarget(dist, "logs")
  description = "srcML package logs for ${dist.name}"
  labels = {
    "org.opencontainers.image.title" = "srcML ${dist.name} Test Logs"
    "org.opencontainers.image.description" = <<EOF
The srcML test logs for ${dist.name}.
EOF
  }
  matrix = {
    dist = distributions
  }
  dockerfile-inline = <<EOF
${builderStage(dist)}
FROM scratch AS dist
COPY --from=builder /src-build/dist/*.log /
EOF
  tags      = [categoryTagName(dist, "logs")]
  output    = ["type=local,dest=${BAKE_DESTINATION_DIR}"]
  inherits  = ["base"]
}

# Specific distribution
function "distribution" {
  params = [id]
  result = [ for item in distributions : item if item.id == id ]
}

# Target name
function "targetName" {
  params = [item]
  result = baseTargetName(item.id, item.version_id)
}

# Target name with a category
function "categoryTarget" {
  params = [item, category]
  result = baseTargetName(item.id, appendCategory(item.version_id, category))
}

# Target name utility
function "baseTargetName" {
  params = [id, version_id]
  result = format("%s_%s", id, sanitize(version_id))
}

# Tag name
function "tagName" {
  params = [item]
  result = baseTagName(BAKE_REGISTRY, item.id, item.version_id)
}

# Tag name with a category
function "categoryTagName" {
  params = [item, category]
  result = baseTagName(BAKE_PACKAGE_REGISTRY, appendCategory(item.id, category), item.version_id)
}

# Tag name with a category
function "baseTagName" {
  params = [registry, id, version_id]
  result = format("${registry != "" ? "${registry}/" : ""}srcml/%s:%s", id, version_id)
}

# Add a category to the id
function "appendCategory" {
  params = [id, category]
  result = format("%s_%s", id, category)
}
