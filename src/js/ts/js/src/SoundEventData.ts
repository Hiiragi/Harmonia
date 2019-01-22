class SoundEventData {

    private rid: string;
    private sid: string;
    private type: SoundEventType;

    constructor(registeredSoundId: string, soundId: string, type: SoundEventType) {
        this.rid = registeredSoundId;
        this.sid = soundId;
        this.type = type;
    }

    public getRid(): string {
        return this.rid;
    }

    public getSid(): string {
        return this.sid;
    }

    public getType(): SoundEventType {
        return this.type;
    }

}
