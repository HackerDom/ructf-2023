#!/usr/bin/env python3


SERVICES = [
    "example", 
    "funficfy",
    "hyperborea-legends",
    "memos", 
    "perune", 
    "rusi-trainer",
    "rustest",
    "sqlfs",
    "werk",
    "wise-mystical-tree",
]

CHECKERS = [
    "example", 
    "funficfy",
    "hyperborea-legends",
    "memos", 
    "perune", 
    "rusi-trainer",
    "rustest",
    "sqlfs",
    "werk",
    "wise-mystical-tree",
]

SPLOITS = [
    "example", 
    "funficfy",
    "hyperborea-legends",
    "memos", 
    "perune", 
    "rusi-trainer",
    "rustest",
    "sqlfs",
    "werk",
    "wise-mystical-tree",
]

TEMPLATE = """
name: Check {service}
on:
  push:
    branches:
      - master
    paths:
      - 'services/{service}/**'
      - 'checkers/{service}/**'
      - 'sploits/{service}/**'
  workflow_dispatch: {{}}
jobs:
  check_service_{service}:
    name: Check service {service}
    runs-on: ubuntu-20.04
    steps:
    - name: Checkout repo
      uses: actions/checkout@v2
    - name: Setup {service}
      run: (cd ./services/{service} && docker-compose pull && docker-compose build && docker-compose up --build -d)
  check_checker_{service}:
    name: Check checker {service}
    runs-on: ubuntu-20.04
    if: {checker_enabled}
    steps:
    - name: Checkout repo
      uses: actions/checkout@v2
    - name: Setup {service}
      run: (cd ./services/{service} && docker-compose pull && docker-compose build && docker-compose up --build -d)
    - name: Sleep some time. Wait service ready
      run: sleep 30
    - name: Prepare python for checker
      uses: actions/setup-python@v2
      with:
        python-version: 3.11
    - name: Setup checker libraries
      run: if [ -f checkers/{service}/requirements.txt ]; then python -m pip install -r checkers/{service}/requirements.txt; fi
    - name: Test checker on service
      run: (cd ./checkers/{service} && ./{service}.checker.py TEST 127.0.0.1)
  check_sploit_{service}:
    name: Check sploit {service}
    runs-on: ubuntu-20.04
    if: {sploit_enabled}
    steps:
    - name: Checkout repo
      uses: actions/checkout@v2
    - name: Setup {service}
      run: (cd ./services/{service} && docker-compose pull && docker-compose build && docker-compose up --build -d)
    - name: Sleep some time. Wait service ready
      run: sleep 30
    - name: Prepare python for checker
      uses: actions/setup-python@v2
      with:
        python-version: 3.11
    - name: Setup checker libraries
      run: if [ -f checkers/{service}/requirements.txt ]; then python -m pip install -r checkers/{service}/requirements.txt; fi
    - name: Setup sploit libraries
      run: if [ -f sploits/{service}/requirements.txt ]; then python -m pip install -r sploits/{service}/requirements.txt; fi
    - name: Test sploit on service
      run: (./tools/check_sploit.py {service})
"""[1:]


def template_bool(var: bool) -> str:
    return "${{ true }}" if var else "${{ false }}"


for service in SERVICES:
    with open(f"check_{service}.yml", "w") as f:
        template = TEMPLATE.format(
            service=service,
            service_lower=service.lower(),
            checker_enabled=template_bool(service in CHECKERS),
            sploit_enabled=template_bool(service in SPLOITS),
        )
        f.write(template)

