#!/bin/bash

set -xeuo pipefail

# https://stackoverflow.com/a/246128
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

repo_dir=$script_dir
build_dir=$repo_dir/build
cpu_count=$(nproc)

mkdir -p "$build_dir"
cd "$build_dir"

cmake_build_type=Release

usage() {
    echo "usage: $0 [-d]" 1>&2
    exit 1
}

# make_opts="--debug=j"
cmake_opts=""

while getopts "dt" opt; do
    case "${opt}" in
        d)
            echo "+ enable debug mode"
            cmake_build_type=Debug
            ;;
        t)
            echo "+ enable cmake trace"
            cmake_opts="$cmake_opts --trace --debug-find"
            ;;
        *)
            usage
            ;;
    esac
done

# The DCMAKE_BUILD_TYPE is appended to CMAKE_C_FLAGS so in this case we get
# CMAKE_C_FLAGS_DDR_RELEASE which corresponds to flags set in the flags.cmake file
cmake $cmake_opts -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=$cmake_build_type "$repo_dir"
make -j"$cpu_count"

cd -
