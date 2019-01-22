class SoundGroup extends AbstractSoundData {

    private soundDataList: AbstractSoundData[];
    private stoppedSoundDataList: PlayingData[];

    private maxPolyphony: number;
    private currentPolyphony: number;

    private tempBufferList: number[][];

    constructor(id: string, maxPolyphony: number = 0) {
        super(id);

        this.soundDataList = [];
        this.stoppedSoundDataList = [];

        this.maxPolyphony = maxPolyphony;
        this.currentPolyphony = 0;
    }

    public addSound(sound: PlayingData): void {
        if (this.maxPolyphony > 0 && this.currentPolyphony === this.maxPolyphony) {
            return;
        }

        this.currentPolyphony++;

        this.addData(sound);
    }

    public addGroup(group: SoundGroup): void {

        this.addData(group);
    }

    public removeSound(sound: PlayingData): void {
        this.currentPolyphony--;

        this.removeData(sound);
    }

    public removeGroup(group: SoundGroup): void {
        this.removeData(group);
    }

    public writeRawPCM(pcmBufferArray: number[][]): void {

        // 再生中以外、あるいはグループによって一時停止されている場合は、何もせずに終了
        if (this.currentStatus !== SoundStatus.PLAYING || this.pausedGroupList.length > 0) {
            return;
        }

        // 停止しているものは削除
        let size = this.stoppedSoundDataList.length;
        if (size > 0) {
            for (let i = 0; i < size; i++) {
                this.stoppedSoundDataList[i].destroy();
            }
            this.stoppedSoundDataList.length = 0;
        }

        const numChannels = pcmBufferArray.length;
        const bufferSize = pcmBufferArray[0].length;

        if (this.tempBufferList == null) {
            this.tempBufferList = new Array(numChannels);
            for (let i = 0; i < numChannels; i++) {
                this.tempBufferList[i] = new Array(bufferSize);
            }
        }

        // リセット
        for (let i = 0; i < numChannels; i++) {
            for (let j = 0; j < bufferSize; j++) {
                this.tempBufferList[i][j] = 0;
            }
        }

        // 書き込み
        for (const data of this.soundDataList) {
            data.writeRawPCM(this.tempBufferList);
        }

        // ボリューム反映
        for (let channel = 0; channel < numChannels; channel++) {
            const buffer: number[] = this.tempBufferList[channel];
            for (let sample = 0; sample < bufferSize; sample++) {
                buffer[sample] *= this.currentVolume;
            }
        }

        // エフェクトコマンド実行
        size = this.effectCommandList.length;
        if (size > 0) {
            for (let i = 0; i < size; i++) {
                this.effectCommandList[i].execute(this.tempBufferList);
            }
        }

        // 合算
        for (let channel = 0; channel < numChannels; channel++) {
            const outputData = pcmBufferArray[channel];
            const wroteData = this.tempBufferList[channel];
            for (let sample = 0; sample < bufferSize; sample++) {
                outputData[sample] += wroteData[sample];
            }
        }

    }

    public pause(): void {
        if (this.currentStatus !== SoundStatus.PLAYING) {
            return;
        }

        this.currentStatus = SoundStatus.PAUSED;
        this._pauseByGroup(this.id);
    }

    public resume(): void {
        if (this.currentStatus !== SoundStatus.PAUSED) {
            return;
        }

        this.currentStatus = SoundStatus.PLAYING;
        this._resumeByGroup(this.id);
    }

    public stop(): void {
        const size = this.soundDataList.length;
        if (size > 0) {
            for (let i = 0; i < size; i++) {
                this.soundDataList[i].stop();
            }
        }
    }

    public mute(): void {
        if (this.isMuted) {
            return;
        }

        this.isMuted = true;

        this._muteByGroup(this.id);
    }

    public unmute(): void {
        if (!this.isMuted) {
            return;
        }

        this.isMuted = false;

        this._unmuteByGroup(this.id);
    }

    public getCurrentPolyphony(): number {
        return this.currentPolyphony;
    }

    public setDucker(triggerGroup: SoundGroup, ratio: number, attackTimeByMS: number = 0, releaseTimeByMS: number = 0) {
        this.effectCommandList.push(new DuckerCommand(triggerGroup, ratio, attackTimeByMS, releaseTimeByMS));
    }

    public _pauseByGroup(soundGroupId: string): void {
        const size: number = this.soundDataList.length;
        for (let i = 0; i < size; i++) {
            this.soundDataList[i]._pauseByGroup(soundGroupId);
        }
    }

    public _resumeByGroup(soundGroupId: string): void {
        const size: number = this.soundDataList.length;
        for (let i = 0; i < size; i++) {
            this.soundDataList[i]._resumeByGroup(soundGroupId);
        }
    }

    public _muteByGroup(soundGroupId: string): void {
        const size: number = this.soundDataList.length;
        for (let i = 0; i < size; i++) {
            this.soundDataList[i]._muteByGroup(soundGroupId);
        }
    }

    public _unmuteByGroup(soundGroupId: string): void {
        const size: number = this.soundDataList.length;
        for (let i = 0; i < size; i++) {
            this.soundDataList[i]._unmuteByGroup(soundGroupId);
        }
    }

    public _registerStoppedSound(playingData: PlayingData): void {
        this.stoppedSoundDataList.push(playingData);
    }

    private addData(data: AbstractSoundData) {
        this.soundDataList.push(data);
        data.setParent(this.id);

        // ミュート状態、ポーズ状態を継承
        if (this.mutedGroupList.length > 0) {
            const size: number = this.mutedGroupList.length;
            for (let i: number = 0; size; i++) {
                data._muteByGroup(this.mutedGroupList[i]);
            }
        }

        if (this.pausedGroupList.length > 0) {
            const size: number = this.pausedGroupList.length;
            for (let i: number = 0; size; i++) {
                data._pauseByGroup(this.pausedGroupList[i]);
            }
        }

        if (this.isMuted) {
            data._muteByGroup(this.id);
        }
        if (this.currentStatus === SoundStatus.PAUSED) {
            data._pauseByGroup(this.id);
        }
    }

    private removeData(data: AbstractSoundData) {
        const index: number = this.soundDataList.indexOf(data);
        this.soundDataList.splice(index, 1);

        data.setParent(null);
    }

}
