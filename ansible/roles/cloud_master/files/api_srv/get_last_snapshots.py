#!/usr/bin/python3
# Developed by Alexander Bersenev from Hackerdom team, bay@hackerdom.ru

"""Lists all vm snapshots"""

import do_api
import sys

from do_tokens import DO_TOKENS

def main():
    pattern = sys.argv[1]
    for token_name, token in DO_TOKENS.items():
        snapshots = do_api.list_snapshots(token)
        snapshots.sort(key=lambda v: v.get("created_at", 0))
        snapshots = list(filter(lambda snapshot: pattern in snapshot["name"] , snapshots))
        if len(snapshots) > 0:
            print(snapshots[-1]["id"])
            return 0

    return 0

    
if __name__ == "__main__":
    main()
