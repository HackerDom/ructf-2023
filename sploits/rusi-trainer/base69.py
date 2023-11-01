chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/-*<>|'


def byteToChars(n):
    return f"{chars[n % 69]}{chars[n // 69]}"


def charsToByte(s):
    return (69 * chars.index(s[1])) + chars.index(s[0])


def encodeArrayWithLength(bytes, startIndex, length, codes):
    endIndex = startIndex + length
    for i in range(startIndex, endIndex):
        shift = (i % 7) + 1
        shifted = bytes[i] >> shift
        if shift > 1:
            pre = (bytes[i - 1] & ((2 << (shift - 2)) - 1)) << (8 - shift)
            shifted = pre | shifted
        codes.append(byteToChars(shifted))
        if shift == 7:
            shifted = bytes[i] & 127
            codes.append(byteToChars(shifted))


def decodeChunk(s):
    paddedBytes = s.endswith('=')
    decoded = bytearray([0] * 8)
    for i in range(8):
        decoded[i] = 0 if (i == 7 and paddedBytes) else charsToByte(s[i * 2:i * 2 + 2])
    result = bytearray([0] * 7)
    for i in range(7):
        t1 = decoded[i] << (i + 1)
        t2 = decoded[i + 1] >> (7 - i - 1)
        result[i] = t1 | t2
    return result


def encode(bytes):
    codes = []
    length = len(bytes)
    extraBytes = length % 7
    encodeArrayWithLength(bytes, 0, length - extraBytes, codes)
    if extraBytes:
        extra = bytearray([0] * 7)
        extra[0:extraBytes] = bytes[length - extraBytes:]
        encodeArrayWithLength(extra, 0, len(extra), codes)
        codes[-1] = f'{7 - extraBytes}='
    return ''.join(codes)


def decode(value):
    extraBytes = 0
    if value[-1] == '=':
        extraBytes = int(value[-2])
    chunkCount = len(value) // 16
    bytes = bytearray((chunkCount * 7) - extraBytes)
    for i in range(chunkCount):
        chunkString = value[i * 16:(i + 1) * 16]
        if extraBytes and (i == chunkCount - 1):
            bytes[i * 7:(i * 7) + (7 - extraBytes)] = decodeChunk(chunkString)[:7 - extraBytes]
        else:
            bytes[i * 7:(i + 1) * 7] = decodeChunk(chunkString)
    return bytes


def encodeString(value):
    bytes = value.encode('utf-8')
    return encode(bytes)


def decodeString(value):
    bytes = decode(value)
    return bytes.decode('utf-8')
