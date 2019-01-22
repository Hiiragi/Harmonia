class AbstractSoundData {

    protected id: string;
    protected isMuted: boolean;
    protected currentStatus: SoundStatus;
    protected currentVolume: number;
    protected mutedGroupList: string[];
    protected pausedGroupList: string[];
    protected effectCommandList: AbstractEffectCommand[];

    protected parentGroupId: string;

    constructor(id: string) {
        this.id = id;
        this.isMuted = false;
        this.currentStatus = SoundStatus.PLAYING;
        this.currentVolume = 1.0;

        this.mutedGroupList = [];
        this.pausedGroupList = [];
        this.effectCommandList = [];
    }

    public getId(): string {
        return this.id;
    }

    // tslint:disable-next-line:no-empty
    public writeRawPCM(pcmBufferArray: number[][]): void {}

    // tslint:disable-next-line:no-empty
    public pause(): void {}

    // tslint:disable-next-line:no-empty
    public resume(): void {}

    // tslint:disable-next-line:no-empty
    public stop(): void {}

    // tslint:disable-next-line:no-empty
    public mute(): void {}

    // tslint:disable-next-line:no-empty
    public unmute(): void {}

    public getVolume(): number {
        return this.currentVolume;
    }

    public setVolume(volume: number): void {
        this.currentVolume = volume;
    }

    public getParent(): string {
        return this.parentGroupId;
    }

    public setParent(parentGroupId: string) {
        this.parentGroupId = parentGroupId;
    }

    // tslint:disable-next-line:no-empty
    public _pauseByGroup(soundGroupId: string) {}

    // tslint:disable-next-line:no-empty
    public _resumeByGroup(soundGroupId: string) {}

    // tslint:disable-next-line:no-empty
    public _muteByGroup(soundGroupId: string) {}

    // tslint:disable-next-line:no-empty
    public _unmuteByGroup(soundGroupId: string) {}
}
