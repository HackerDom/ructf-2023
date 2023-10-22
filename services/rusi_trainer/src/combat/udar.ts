export interface UdarDto {
    /**
     * @isInt provide a number here
     * @minimum 1 minimum age is 0
     * @maximum 4 maximum age is 4
     */
    map: number;
    /**
     * @maxItems 3 max length of Rusi is 3
     * @isArray  Wrong format
     * @uniqueItems Duplicate keys
     */
    trustedRusi: number[]
    /**
     * @maxLength 500 max length of description is 500
     */
    description: string;
    /**
     * @maxLength 255 max length of phonk is 255
     */
    phonk: string;

    /**
     * @maxLength 255 max length of name is 255
     */
    name: string;
}

