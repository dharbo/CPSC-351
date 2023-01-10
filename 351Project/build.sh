#!/usr/bin/env bash
executableFileName="${1:-cpsc351Program}"


# Find and display all the c source files to be compiled ...
# temporarily ignore spaces when globing words into file names
temp=$IFS
  IFS=$'\n'  
  sourceFiles=( $(find ./ -name "*.c") )
IFS=$temp

echo "compiling ..."
for fileName in "${sourceFiles[@]}"; do
  echo "  $fileName"
done
echo ""

# Set some options and perform the compolations ...
options="-g3 -O0 -pthread  -I./ -DUSING_TOMS_SUGGESTIONS"
gcc $options -o "$executableFileName" "${sourceFiles[@]}" -lrt && echo -e "\nSuccessfully created  \"$executableFileName\""
