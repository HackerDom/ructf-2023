export class AuthError extends Error {
    constructor(message?: string) {
        super(message);
    }
}

export class BadRequestError extends Error {
    constructor(message?: string) {
        super(message);
    }
}