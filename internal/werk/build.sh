#!/usr/bin/env bash

set -ue

show_help() {
  cat >&2 <<EOF
Usage: $script_name COMMAND
Command:
    run-tests-devel
    run-tests-release
    build-release
    build-devel
    install-binaries
    clean
EOF
}

do_install_binaries() {
    cp build/asm/libasm.so ../../services/werk
    cp build/common/libcommon.so ../../services/werk
    cp build/server/interpreter_server ../../services/werk
}

do_build_devel() {
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j7
}

do_build_release() {
    mkdir -p build && cd build
    cmake -DCMAKE_BUILD_TYPE=Release .. && make -j7
}

do_clean() {
    mkdir -p build && cd build
    make clean
}

do_run_tests_devel() {
    do_build_devel
    find . -type f -executable -name "*_tests" -exec /usr/bin/env bash -c 'echo "Running tests from: {}"; {}' \;
}

do_run_tests_release() {
    do_build_release
    find . -type f -executable -name "*_tests" -exec /usr/bin/env bash -c 'echo "Running tests from: {}"; {}' \;
}

main() {
    if [ $# -lt 1 ]; then
        show_help
        exit 1
    fi

    case "$1" in
    run-tests-devel)
        shift
        do_run_tests_devel "$@"
        ;;
    run-tests-release)
        shift
        do_run_tests_release "$@"
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
    install-binaries)
        shift
        do_install_binaries "$@"
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

