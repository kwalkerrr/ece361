#!/bin/sh
fail=0
for input in inputs/*; do
    name=$(basename "$input")
    if ./balance < "$input" | diff - "expected/$name" > /dev/null; then
        echo "PASS  regression $name"
    else
        echo "FAIL  regression $name"
        fail=1
    fi
done
exit $fail
