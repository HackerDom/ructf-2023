const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/-*<>|';
export const BestChars: string = 'abikl';
// @ts-ignore
function byteToChars({n}) {
    return `${chars[n % 69]}${chars[Math.floor(n / 69)]}`;
}
// @ts-ignore
function charsToByte({s}) {
    return (69 * chars.indexOf(s.charAt(1))) + chars.indexOf(s.charAt(0));
}

// @ts-ignore
function encodeArrayWithLength(bytes, startIndex, length, codes) {
    const endIndex = startIndex + length;
    for (let i = startIndex; i < endIndex; i++) {
        const shift = (i % 7) + 1;
        let shifted = bytes[i] >> shift;
        if (shift > 1) {
            const pre = (bytes[i - 1] & ((2 << (shift - 2)) - 1)) << (8 - shift);
            shifted = pre | shifted;
        }
        codes.push(byteToChars({n: shifted}));
        if (shift == 7) {
            shifted = bytes[i] & 127;
            codes.push(byteToChars({n: shifted}));
        }
    }
}
// @ts-ignore
function decodeChunk(s) {
    const paddedBytes = s.endsWith('=');
    const decoded = new Uint8Array(8);
    for (let i = 0; i < 8; i++) {
        decoded[i] = (i === 7 && paddedBytes) ? 0 : charsToByte({s: s.substring(i * 2, i * 2 + 2)});
    }
    const result = new Uint8Array(7);
    for (let i = 0; i < 7; i++) {
        let t1 = decoded[i] << (i + 1);
        let t2 = decoded[i + 1] >> (7 - i - 1);
        result[i] = t1 | t2;
    }
    return result;
}

export function encode(bytes: Uint8Array) : string{
    // @ts-ignore
    const codes = [];
    const len = bytes.length;
    const extraBytes = len % 7;
    // @ts-ignore
    encodeArrayWithLength(bytes, 0, len - extraBytes, codes);
    if (extraBytes) {
        const extra = new Uint8Array(7);
        extra.fill(0, 0);
        extra.set(bytes.slice(len - extraBytes), 0);
        // @ts-ignore
        encodeArrayWithLength(extra, 0, extra.length, codes);
        codes[codes.length - 1] = `${7 - extraBytes}=`;
    }
    return codes.join('');
}

export function decode(value: string) : Uint8Array {
    let extraBytes = 0;
    if (value.charAt(value.length - 1) === '=') {
        extraBytes = +value.charAt(value.length - 2);
    }
    const chunkCount = Math.ceil(value.length / 16);
    const bytes = new Uint8Array(chunkCount * 7 - extraBytes);
    for (let i = 0; i < chunkCount; i++) {
        const chunkString = value.substring(i * 16, (i + 1) * 16);
        if (extraBytes && (i == chunkCount - 1)) {
            bytes.set(decodeChunk(chunkString).subarray(0, 7 - extraBytes), i * 7);
        }
        else {
            bytes.set(decodeChunk(chunkString), i * 7);
        }
    }
    return bytes;
}