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
  default = "https://github.com/srcML/srcML.git#v1.1.0-beta"
}

# Override using the environment variable BAKE_PRESET_SUFFIX
# E.g., BAKE_PRESET_SUFFIX="-fast"
variable "BAKE_PRESET_SUFFIX" {
  # description = "Suffix for the workflow preset"
  default = ""
}

# Override using the environment variable BAKE_ARCHITECTURE
# E.g., BAKE_ARCHITECTURE="linux/arm64"
variable "BAKE_ARCHITECTURE" {
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

# Override using the environment variable BAKE_CACHE
# E.g., BAKE_CACHE=type=local,src=bake_cache,dest=bake_cache
# E.g., BAKE_CACHE=""
variable "BAKE_CACHE" {
  # description = "Cache for build layers"
  default = ""
}

# Placeholder for distributions. See docker-bake.override.hcl for configured
# Linux distributions and distribution-specific groups and targets
variable "distributions" {
  default = [ { id = "ubuntu", version_id = "24.04", java_version_id = "22.04", name = "Ubuntu 24.04", workflow = "ubuntu", java = "latest" } ]
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
  platforms = split(",", BAKE_ARCHITECTURE)
  labels = {
    "org.opencontainers.image.version"  = BAKE_SRCML_VERSION
    "org.opencontainers.image.licenses" = "GPL-3.0-only"
  }
  pull = true
  cache-to = [ BAKE_CACHE ]
  cache-from = [ BAKE_CACHE ]
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
    TAG          = dist.version_id
    JAVA_TAG     = dist.java_version_id
    CMAKE_VERSION = BAKE_CMAKE_VERSION
    CMAKE_BINARY = try(dist.cmake, "")
    JAVA_VERSION = try(dist.java, "")
    OPENSUSE     = try(dist.opensuse, "")
  }
  tags     = concat([tagName(dist), tagNameAlias(dist)])
  inherits = ["base"]
  # cache-from = ["type=local,src=./.docker-build-cache/${targetName(dist)}"]
  # cache-to   = ["type=local,dest=./.docker-build-cache/${targetName(dist)},mode=max"]
}

# Dockerfile to build the package
# The overall workflow preset is separated into the various stages. That way if the cache
# for a layer is invalidated, it is not the entire workflow. The three testing stages all
# are from the result of the package and install, and can be done in parallel.
# The use of multiple stages allows for fine-grained cache invalidation
function "builderStage" {
  params = [dist]
  result = <<EOF
FROM ${tagName(dist)} AS builder
WORKDIR /src
ADD --link ["${BAKE_SRC}", "."]
# RUN cmake --workflow --preset ${workflowPreset(dist)}
RUN cmake --preset ${workflowPreset(dist)}
RUN cmake --build --preset ${workflowPreset(dist)}
FROM builder AS packager
RUN cpack --preset ${workflowPreset(dist)}
RUN cmake --build --preset ci-install-package-${dist.workflow}
FROM packager AS test_client
RUN cmake --build --preset ci-test-client-${dist.workflow}
# FROM packager AS test_libsrcml
# RUN cmake --build --preset ci-test-libsrcml-${dist.workflow}
FROM packager AS setup_parser_tests
RUN cmake --build --preset ci-test-parser-${dist.workflow}
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
COPY --from="packager" \
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
COPY --from=packager /src-build/dist/*.log /
EOF
  tags      = [categoryTagName(dist, "logs")]
  output    = ["type=local,dest=${BAKE_DESTINATION_DIR}"]
  inherits  = ["base"]
}

target "examples" {
  name = categoryTarget(dist, "examples")
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
FROM ${tagName(dist)} AS examples
COPY --from="packager" \
  /src-build/dist/*.rpm \
  /src-build/dist/*.deb \
  /src-build/dist/*.tar.gz \
  /src-build/dist/*.bz2 \
  /
RUN apt-get install /*.deb
EOF
  tags     = [categoryTagName(dist, "examples")]
  # output   = ["type=local,dest=${BAKE_DESTINATION_DIR}"]
  inherits = ["base"]
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

# Tag name
function "tagNameAlias" {
  params = [item]
  result = baseTagName(BAKE_REGISTRY, item.id, try(item.tag, item.version_id))
}

# Add a category to the id
function "appendCategory" {
  params = [id, category]
  result = format("%s_%s", id, category)
}
