#!/usr/bin/env bash

for file in `curl https://tablebase.lichess.ovh/tables/standard/3-4-5/ -s | grep -E '"\w*\.rtb."' -o | grep -E '\w*\.rtb.' -o`
do
    echo "$file"
    curl "https://tablebase.lichess.ovh/tables/standard/3-4-5/$file" --output "$file" -s
done

echo "Done!"
