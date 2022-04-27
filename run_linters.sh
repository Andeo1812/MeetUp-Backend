#!/usr/bin/env bash

set -o pipefail

function print_header() {
    echo -e "\n***** ${1} *****"
}

function check_log() {
    LOG=$( { ${1}; } 2>&1 )
    STATUS=$?
    echo "$LOG"
    if echo "$LOG" | grep -q -E "${2}"
    then
        exit 1
    fi

    if [ $STATUS -ne 0 ]
    then
        exit $STATUS
    fi
}

main="project/main.cpp"

Context_hpp="project/context/include/*"
Context_cpp="project/context/src/*"
Context_path_hpp="-I project/context/include"

Database_hpp="project/database/include/*/*"
Database_cpp="project/database/PostgreSQL/src/*"
Database_path_hpp="-I project/database/include/virtual -I project/database/include/impl -I project/database/PostgreSQL/include"

Parsing_hpp="project/parsing/include/impl/*.hpp project/parsing/include/impl/Data/* project/parsing/include/impl/Group/* project/parsing/include/virtual/*"
Parsing_cpp="project/parsing/JSON/src/*.cpp project/parsing/JSON/src/Data/* project/parsing/JSON/src/Group/*"
Parsing_path_hpp="-I project/parsing/include/impl -I project/parsing/include/impl/Data -I project/parsing/include/impl/Group -I project/parsing/include/virtual -I project/parsing/JSON/libs/single_include/nlohmann"

Parsing_path_hpp_without_lib="-I project/parsing/include/impl -I project/parsing/include/impl/Data -I project/parsing/include/impl/Group -I project/parsing/include/virtual"

Handling_hpp="project/handling/include/impl/*.hpp project/handling/include/impl/Data/* project/handling/include/impl/Group/* project/handling/include/virtual/* project/handling/src/Group/MeetUp/SupportMeetUp/include/*"
Handling_cpp="project/handling/src/*.cpp project/handling/src/Data/* project/handling/src/Group/Base/* project/handling/src/Group/User/* project/handling/src/Group/MeetUp/*.cpp project/handling/src/Group/MeetUp/SupportMeetUp/src/*"
Handling_path_hpp="-I project/handling/include/impl -I project/handling/include/impl/Data -I project/handling/include/impl/Group -I project/handling/include/virtual -I project/handling/src/Group/MeetUp/SupportMeetUp/include"

Routing_hpp="project/routing/include/impl/* project/routing/include/virtual/*"
Routing_cpp="project/routing/src/*"
Routing_path_hpp="-I project/routing/include/impl -I project/routing/include/virtual"

Sum_hpp="${Context_hpp} ${Database_hpp} ${Parsing_hpp} ${Handling_hpp} ${Routing_hpp}"
Sum_cpp="${main} ${Context_cpp} ${Database_cpp} ${Parsing_cpp} ${Handling_cpp} ${Routing_cpp}"

Sum_path_hpp_base="${Context_path_hpp} ${Database_path_hpp} ${Handling_path_hpp} ${Routing_path_hpp}"

Sum_path_hpp="${Sum_path_hpp_base} ${Parsing_path_hpp}"
Sum_path_hpp_without_lib="${Sum_path_hpp_base} ${Parsing_path_hpp_without_lib}"

print_header "RUN cppcheck"
check_log "cppcheck project --enable=all --check-config --inconclusive --error-exitcode=1 ${Sum_path_hpp_without_lib} --suppress=missingIncludeSystem" "\(information\)"

print_header "RUN clang-tidy"
check_log "clang-tidy ${Sum_cpp} ${Sum_hpp} -warnings-as-errors=*  -- -x c++ ${Sum_path_hpp}" "Error (?:reading|while processing)"

print_header "RUN cpplint"
check_log "cpplint --extensions=cpp ${Sum_cpp} ${Sum_hpp}" "Can't open for reading"

print_header "SUCCESS"
