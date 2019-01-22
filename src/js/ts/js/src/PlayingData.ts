class PlayingData extends AbstractSoundData {

    private currentPosition: number;
    private registerSoundData: RegisteredSoundData;
    private playingStatus: PlayingStatus;
    private slicedArray: Float32Array;
    private playingTimeByLoop: number;
    private bufferArray: number[][];

    constructor(registerSoundData: RegisteredSoundData, soundId: string) {
        super(soundId);

        this.registerSoundData = registerSoundData;
        this.currentPosition = 0;
    }

    public writeRawPCM(pcmBufferArray: number[][]): void {

        // 再生中以外、あるいはグループによって一時停止されている場合は、何もせずに終了
        if (this.currentStatus !== SoundStatus.PLAYING || this.pausedGroupList.length > 0) {
            return;
        }

        const numChannels = pcmBufferArray.length;
        const bufferSize = pcmBufferArray[0].length;

        if (this.bufferArray == null) {
            this.bufferArray = new Array(numChannels);
            for (let i = 0; i < numChannels; i++) {
                this.bufferArray[i] = new Array(bufferSize);
            }
        }

        for (let channel = 0; channel < numChannels; channel++) {

            let channelData;
            // mono/stereo 対応
            if (this.registerSoundData.numberOfChannels === 1) {
                channelData = this.getChannelData(0, this.currentPosition, bufferSize);
            } else {
                channelData = this.getChannelData(channel, this.currentPosition, bufferSize);
            }

            for (let sample = 0; sample < bufferSize; sample++) {
                this.bufferArray[channel][sample] = channelData[sample] * this.currentVolume;
            }
        }

        // エフェクトコマンド実行
        const effectCommandLength: number = this.effectCommandList.length;
        if (effectCommandLength > 0) {
            for (let i = 0; i < effectCommandLength; i++) {
                this.effectCommandList[i].execute(this.bufferArray);
            }
        }

        // 合算
        for (let channel = 0; channel < numChannels; channel++) {

            const channelData = this.bufferArray[channel];

            for (let sample = 0; sample < bufferSize; sample++) {
                pcmBufferArray[channel][sample] += channelData[sample];
            }
        }

        if (this.playingStatus === PlayingStatus.NORMAL) {
            this.currentPosition += bufferSize;
        } else if (this.playingStatus === PlayingStatus.LOOPED) {
            this.currentPosition = this.playingTimeByLoop;
        } else if (this.playingStatus === PlayingStatus.END) {
            this.currentPosition = this.registerSoundData.length;
            this.stop();
            Harmonia._setCapturedEvent(this.registerSoundData.getId(), this.id, SoundEventType.SOUND_PLAY_COMPLETE);
        }

        // console.log(this.currentPosition);
    }

    public pause(): void {
        console.log("pause() / id : " + this.id);
        if (this.currentStatus === SoundStatus.PLAYING) {
            this.currentStatus = SoundStatus.PAUSED;
        }
    }

    public resume(): void {
        console.log("resume() / id : " + this.id);
        if (this.currentStatus === SoundStatus.PAUSED) {
            this.currentStatus = SoundStatus.PLAYING;
        }
    }

    public stop(): void {
        console.log("stop() / id : " + this.id);
        this.currentStatus = SoundStatus.STOPPED;

        // 削除
        const parentGroup = Harmonia.getGroup(this.parentGroupId);
        parentGroup._registerStoppedSound(this);
    }

    public mute(): void {
        console.log("mute() / id : " + this.id);
        this.isMuted = true;
    }

    public unmute(): void {
        console.log("unmute() / id : " + this.id);
        this.isMuted = false;
    }

    public destroy(): void {
        Harmonia._removePlayingData(this);

        const parentGroup: SoundGroup = Harmonia.getGroup(this.parentGroupId);
        parentGroup.removeSound(this);
    }

    public getCurrentStatus(): number {
        return this.currentStatus;
    }

    public getCurrentPosition(): number {
        return this.currentPosition;
    }

    public getRegisteredSoundId(): string {
        return this.registerSoundData.getId();
    }
    public _pauseByGroup(soundGroupId: string) {
        this.pausedGroupList.push(soundGroupId);
    }

    public _resumeByGroup(soundGroupId: string) {
        const index: number = this.pausedGroupList.indexOf(soundGroupId);
        if (index >= 0) {
            this.pausedGroupList.splice(index, 1);
        }
    }

    public _muteByGroup(soundGroupId: string) {
        this.mutedGroupList.push(soundGroupId);
    }

    public _unmuteByGroup(soundGroupId: string) {
        const index: number = this.mutedGroupList.indexOf(soundGroupId);
        if (index >= 0) {
            this.mutedGroupList.splice(index, 1);
        }
    }

    private getChannelData(channel: number, position: number, size: number): Float32Array {
        const source: Float32Array = this.registerSoundData.buffers[channel];

        if (this.slicedArray != null) {
            this.slicedArray.fill(0);
        } else {
            this.slicedArray = new Float32Array(size);
        }

        let len: number;

        const loopStartPoint = this.registerSoundData.loopStartPoint;
        const loopLength = this.registerSoundData.loopLength;
        const length = this.registerSoundData.length;

        const isMuted = this.isMuted || this.mutedGroupList.length > 0;

        if (loopLength > 0 && position + size > loopStartPoint + loopLength) {
            // ループありで、ループポイントからはみ出る
            // console.log("ループありで、ループポイントからはみ出る");

            let i = 0;
            if (!isMuted) {
                len = loopStartPoint + loopLength - position;
                for (i; i < len; i++) {
                    this.slicedArray[i] = source[position + i] * this.currentVolume;
                }
            }

            let j = 0;
            if (!isMuted) {
                len = size - len;
                for (j; j < len; j++) {
                    this.slicedArray[i++] = source[loopStartPoint + j] * this.currentVolume;
                }
            }

            this.playingTimeByLoop = loopStartPoint + j;
            this.playingStatus = PlayingStatus.LOOPED;
        } else if (position + size > length) {
            if (position <= length) {
                // 後ろにはみ出る
                // console.log("後ろにはみ出る");
                if (!isMuted) {
                    len = length - position;
                    for (let i = 0; i < len; i++) {
                        this.slicedArray[i] = source[position + i] * this.currentVolume;
                    }
                }
                this.playingStatus = PlayingStatus.END;
            }
        } else {
            // それ以外
            if (!isMuted) {
                len = this.slicedArray.length;
                for (let i = 0; i < len; i++) {
                    this.slicedArray[i] = source[position + i] * this.currentVolume;
                }
            }
            this.playingStatus = PlayingStatus.NORMAL;
        }

        return this.slicedArray;
    }

}

enum PlayingStatus {
    NORMAL,
    LOOPED,
    END,
}
