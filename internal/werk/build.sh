#!/usr/bin/env bash

set -ue

show_help() {
  cat >&2 <<EOF
Usage: $script_name COMMAND
Command:
    run-tests
    build-release
    build-devel
    clean
EOF
}

do_build_devel() {
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j
}

do_build_release() {
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. && make -j
}

do_clean() {
    mkdir -p build && cd build
    make clean
}

do_run_tests() {
    do_build_devel
    find . -type f -executable -name "*_tests" -exec /usr/bin/env bash -c 'echo "Running tests from: {}"; {}' \;
}

main() {
    if [ $# -lt 1 ]; then
        show_help
        exit 1
    fi

    case "$1" in
    run-tests)
        shift
        do_run_tests "$@"
        ;;
    build-devel)
        shift
        do_build_devel "$@"
        ;;
    build-release)
        shift
        do_build_release "$@"
        ;;
    clean)
        shift
        do_clean "$@"
        ;;
    *)
        show_help
        exit 1
        ;;
    esac
}

script_name="$0"
cd "$(dirname "$0")"

main "$@"

