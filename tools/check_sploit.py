#!/usr/bin/env python3
import sys
import random
import string
import subprocess
import json


SERVICE_NAME = sys.argv[1]
FLAG_ID = "flag-id"
TEST_FLAG = "=" + "".join(random.choice(string.ascii_lowercase) for _ in range(32))


print(f"Checking sploit using flag '{TEST_FLAG}'")
p = subprocess.Popen(
    [f"./{SERVICE_NAME}.checker.py", "PUT", "127.0.0.1", FLAG_ID, TEST_FLAG, "1"],
    cwd=f"checkers/{SERVICE_NAME}",
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
)
flag_data, _ = p.communicate()
print(f"Putted flag '{flag_data}'")

FLAG_ID = json.loads(flag_data)["public_flag_id"]
print(f"Public flag id: {FLAG_ID}")
flag_private = json.loads(flag_data)["private_content"]
print(f"Private flag id: {flag_private}")

p = subprocess.Popen(
    [f"./{SERVICE_NAME}.sploit.py", "127.0.0.1", FLAG_ID],
    cwd=f"sploits/{SERVICE_NAME}",
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
)
text_with_flag, err = p.communicate()
if err:
    raise Exception(f"Sploit error: {err}")
print(f"Text from sploit - {text_with_flag}")

if TEST_FLAG not in str(text_with_flag):
    raise Exception("Flag not found in sploit stdout")

print("Success")
