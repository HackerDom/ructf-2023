#!/usr/bin/env bash
imageid=$( ./get_last_snapshots.py $1)
echo "{ \"main\": $imageid}" > do_vulnimages.json
