#!/bin/bash

src_dir="src"

ocf_path="odi-ocf.c"

c_files=$(find "$src_dir" -name "*.c")

for c_file in $c_files; do
  echo "#include \"$(basename "$c_file")\"" >> "$ocf_path"
done

echo "OCF (One C File) created with success : $ocf_path"
