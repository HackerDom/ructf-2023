#!/usr/bin/env python3
import socket
import re
from typing import Optional

import gornilo

valid_gift_type_menu = b"-- Choose gift --\n0. Egg\n1. Milk\n2. Meat\n3. Porridge\n4. Bread\n5. Orange\n>"
valid_gift_count_entry = b"{?} Enter gift count: "
valid_gift_password_entry = b"{?} Enter password: "
valid_print_gift_id_entry = b"{!} Your gift id: "
valid_enter_gift_id_entry = b"{?} Enter gift ID: "
valid_enter_prayer_text_entry = b"{?} Enter prayer text: "
valid_base_menu = b"---- Menu ----\n1. Make gift\n2. View gift\n3. Make prayer\n4. Call Perune\n5. Exit\n>"
perune_verdict_1 = b"{+} Your prayer {%s} is completed!"
perune_verdict_2 = b"{-} I think you're a lizard! Get out here!"
no_such_object_err = b"{-} No such object!"

uuid_extract_pattern = rb"[0-9a-f]{8}-[0-9a-f]{4}-[0-5][0-9a-f]{3}-[089ab][0-9a-f]{3}-[0-9a-f]{12}"

DEFAULT_RECV_SIZE = 4096
TCP_CONNECTION_TIMEOUT = 10
TCP_OPERATIONS_TIMEOUT = 10

class Prayer:
    def __init__(self, text, id):
        self.text = text
        self.id = id
    
    def set_id(self, id):
        self.id = id


class Gift:
    def __init__(self, type, count, password):
        self.type = int(type)
        self.count = int(count)
        self.password = password
    
    def set_id(self, id):
        self.id = id
    
    def get_id(self):
        return self.id
    
    
class PeruneClient:

    def __init__(self, host, port):
        self.host_ = host
        self.port_ = port
    
    def connection(self):
        self.sock = socket.socket()
        self.sock.settimeout(TCP_CONNECTION_TIMEOUT)
        server = (self.host_, self.port_)
        self.sock.connect(server)
        self.sock.settimeout(TCP_OPERATIONS_TIMEOUT)
    
    def read_menu(self):
        data = self.recvuntil(b"> ")
        if valid_base_menu not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        return data
    
    def sendline(self, data):
        return self.send(data + b'\n')
    
    def send(self, data):
        nbytes = self.sock.send(data)
        return nbytes
    
    def recv(self):
        data = self.sock.recv(DEFAULT_RECV_SIZE)
        return data
    
    def recvuntil(self, until_data):
        data = self.recv()
        while until_data not in data:
            t = self.recv()
            if t == b'':
                break
            data += t
        return data

    def make_gift(self, gift: Gift) -> bytes:
        self.sendline(b"1") # make gift cmd
        data = self.recvuntil(b"> ")

        if valid_gift_type_menu not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        self.sendline(str(gift.type).encode()) # send gift type
        data = self.recvuntil(b": ")
        if valid_gift_count_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        self.sendline(str(gift.count).encode()) # send gift count
        data = self.recvuntil(b": ")
        if valid_gift_password_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        self.sendline(gift.password) # send gift password
        data = self.recvuntil(b"> ")

        if valid_base_menu not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        if valid_print_gift_id_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        # try to get gift id
        matches = re.findall(uuid_extract_pattern, data)

        # check that we found uuid for created gift
        if len(matches) != 1:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        return matches[0] # gift uuid

    def view_gift(self, gift_id: bytes, gift_password: bytes) -> Optional[Gift]:
        self.sendline(b"2") # view gift cmd
        data = self.recvuntil(b": ")

        if valid_enter_gift_id_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        self.sendline(gift_id) # gift id
        data = self.recv()
        
        if no_such_object_err in data:
            return None

        # check invalid gift id
        if valid_gift_password_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        self.sendline(gift_password) # gift password

        data = self.recvuntil(b"> ") # get all gift data and menu banner
        
        if valid_base_menu not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        data = data[0:-len(valid_base_menu)-1].split(b'\n')
        if len(data) < 4:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        try:
            gift_type = data[1].split(b' ')[1].decode()
            gift_count = data[2].split(b' ')[1].decode()
            gift_password = data[3].split(b' ')[1]
        except Exception:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        return Gift(gift_type, gift_count, gift_password)
    
    def make_prayer(self, gift: Gift, text: bytes) -> Optional[Prayer]:
        self.sendline(b"3") # make prayer cmd
        data = self.recvuntil(b": ")
        if valid_enter_gift_id_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        self.sendline(gift.id) # gift id
        data = self.recv()
        
        if no_such_object_err in data:
            return None
        
        if valid_gift_password_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        self.sendline(gift.password) # gift password
        data = self.recvuntil(b": ")
        if valid_enter_prayer_text_entry not in data:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        self.sendline(text) # prayer data

        # get prayer id
        data = self.recvuntil(b"> ") # get id and all default menu
        # try to get gift id
        matches = re.findall(uuid_extract_pattern, data)

        # check that we found uuid for created gift
        if len(matches) != 1:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
        
        return Prayer(text, matches[0])
    
    def call_perune(self, prayer_id: bytes) -> bytes:
        self.sendline(b"4") # call perune cmd
        data = self.recvuntil(b": ")
        #print(data)

        self.sendline(prayer_id)
        data = self.recvuntil(b"> ")
        data = data.split(b'\n')

        if len(data) == 0:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')

        if data[0].startswith(b"{-} I think you're a lizard! Get out here!"):
            return data[0]
        elif data[0].startswith(b"{+} Your prayer {"):
            return data[0]
        else:
            raise gornilo.Verdict.MUMBLE('Invalid service protocol')
    
    def exit(self):
        self.sendline(b"5") # call perune cmd
        self.sock.close()
        self.sock = None
