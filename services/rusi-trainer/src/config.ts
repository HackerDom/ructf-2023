import * as fs from 'fs';
import * as crypto from 'crypto';
const secretsFilePath = './secrets/secrets.txt';

let jwtSecret = 'BAIKAL';

export const JWT_MINUTES_EXPIRES_IN = parseInt(process.env.JWT_MINUTES_EXPIRES_IN || "159");

if (fs.existsSync(secretsFilePath)) {
    jwtSecret = fs.readFileSync(secretsFilePath, 'utf-8');
} else {
    jwtSecret = generateSecret();
    fs.writeFileSync(secretsFilePath, jwtSecret, 'utf-8');
}

export const JWT_SECRET = jwtSecret;

function generateSecret(): string {
    const length = 32;
    return crypto.randomBytes(length).toString('hex');
}