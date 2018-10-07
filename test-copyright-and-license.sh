#!/bin/bash
# 
# Copyright (C) 2005-2018 Jo\u00EBl Kr\u00E4hemann
# 
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.  This file is offered as-is,
# without any warranty.

GNU_GPL_3_0_C=`cat license-notice-gnu-gpl-3-0+-c.txt`
GNU_AGPL_3_0_C=`cat license-notice-gnu-agpl-3-0+-c.txt`

GNU_FDL_1_3_XML=`cat license-notice-gnu-fdl-1-3-xml.txt`
PERMISSIVE_XML=`cat license-notice-permissive-xml.txt`

GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

check_files=()

# check C source files
c_files=`find ./ -name "*.[ch]"`

for f in ${c_files[@]}
do
    echo $f
    success="yes"

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

    if grep -q "$GNU_GPL_3_0_C" $f; then
	printf "license: ${GREEN}OK${NC}\n"
    else
	if grep -q "$GNU_AGPL_3_0_C" $f; then
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

# check XML files
xml_files=( `find ./ -name "*.xml"` `find ./ -name "*.dtd"` `find ./ -name "*.xsl"` `find ./ -name "*.xsd"`)

for f in ${xml_files[@]}
do
    echo $f
    success="yes"

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

    if grep -q "$GNU_FDL_1_3_XML" $f; then
	printf "license: ${GREEN}OK${NC}\n"
    else
	if grep -q "$PERMISSIVE_XML" $f; then
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

# result
echo -e "Summary of suspicious files:\n================================"

# notice to verify file
for f in ${check_files[@]}
do
    dir_path=$(dirname "${f}")
    file_path=$(basename "${f}")

    
    if [ ! -f "${dir_path}/README" ] ; then
	echo "Verify: $f"
    else
	if [ "$file_path" != "README" ] ; then
	    if !(grep -q -E "${file_path} \(Copyright \(C\) 20[0-9][0-9]-20[0-9][0-9] [^\s]+ [^\s]+, GPLv3\+|GFDLv1.3\+" ${dir_path}/README || grep -q -E "${file_path} \(Copyright \(C\) 20[0-9][0-9] [^\s]+ [^\s]+, GPLv3\+|GFDLv1.3\+" ${dir_path}/README) ; then
		echo "Verify: $f"
	    fi
	fi
    fi
done

# filter all files
all_files=( `find ./ -not -path "./.git/*" -type f` )

other_files=" ${all_files[@]} "

# filter C files
for item in ${c_files[@]}; do
  other_files=${other_files/ ${item} / }
done

# filter XML files
for item in ${xml_files[@]}; do
  other_files=${other_files/ ${item} / }
done

# notice to verify file
check_files=( $other_files )

for f in ${check_files[@]}
do
    dir_path=$(dirname "${f}")
    file_path=$(basename "${f}")

    
    if [ ! -f "${dir_path}/README" ] ; then
	echo "Verify: $f"
    else
	if [ "$file_path" != "README" ] ; then
	    if !(grep -q -E "${file_path} \(Copyright \(C\) 20[0-9][0-9]-20[0-9][0-9] [^\s]+ [^\s]+, GPLv3\+|GFDLv1.3\+" ${dir_path}/README || grep -q -E "${file_path} \(Copyright \(C\) 20[0-9][0-9] [^\s]+ [^\s]+, GPLv3\+|GFDLv1.3\+" ${dir_path}/README) ; then
		echo "Verify: $f"
	    fi
	fi
    fi
done

echo "================================"

