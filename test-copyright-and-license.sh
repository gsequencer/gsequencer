#!/bin/bash
# 
# Copyright (C) 2005-2018 Jo\u00EBl Kr\u00E4hemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

# check C source files
files=`find ags/ -name "*.[ch]"`
check_files=()

for f in $files
do
    success="yes"
    echo $f

    if grep -q "Copyright (C) 20[0-9][0-9]-20[0-9][0-9]" $f; then
	printf "copyright: ${GREEN}OK${NC}\n"
    else
	if grep -q "Copyright (C) 20[0-9][0-9]" $f; then
	    printf "copyright: ${GREEN}OK${NC}\n"
	else
	    printf "copyright:  ${RED}missing!${NC}\n"

	    success="no"
	fi
    fi

    if grep -q "You should have received a copy of the GNU General Public License" $f; then
	printf "license: ${GREEN}OK${NC}\n"
    else
	if grep -q "You should have received a copy of the GNU Affero General Public License" $f; then
	    printf "license: ${GREEN}OK${NC}\n"
	else
	    printf "license:  ${RED}missing!${NC}\n"

    	    success="no"
	fi
    fi

    if [ "$success" = "no" ]; then
	check_files+=("$f")
    fi
done

for f in ${check_files[*]}
do
    echo "Verify: $f"
done

# filter other files
all_files=( `find ags/ -type f` )

other_files=" ${all_files[*]} "

for item in ${files[@]}; do
  other_files=${other_files/ ${item} / }
done

check_files=( $other_files )

for f in ${check_files[*]}
do
    echo "Verify: $f"
done
