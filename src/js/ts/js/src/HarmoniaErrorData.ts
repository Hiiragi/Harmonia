class HarmoniaErrorData {

    private type: HarmoniaErrorType;

    constructor(type: HarmoniaErrorType) {
        this.type = type;
    }

    public getErrorType(): HarmoniaErrorType {
        return this.type;
    }
}
