export function omitField<T extends object, K extends keyof T>(obj: T, field: K): Omit<T, K> {
    const { [field]: omitted, ...rest } = obj;
    return rest;
}

export type Overwrite<T, U> = Pick<T, Exclude<keyof T, keyof U>> & U;
