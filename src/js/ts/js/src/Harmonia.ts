class Harmonia {

    // tslint:disable-next-line:member-ordering
    public static initialize(bufferSize: number) {
        if (this.isInitialized) {
            return;
        }

        this.isInitialized = true;

        this.registeredSoundDictionary = {};
        this.soundGroupDictionary = {};
        this.playingDataDictionary = {};

        this.capturedEvents = [];
        this.capturedErrors = [];

        this.masterGroup = new SoundGroup(this.MASTER_GROUP_ID);
        // this.soundGroupDictionary[this.MASTER_GROUP_ID] = this.masterGroup;

        const AudioContextClass = (window as any).AudioContext || (window as any).webkitAudioContext;
        this.audioContext = new AudioContextClass();
        this.sampleRate = this.audioContext.sampleRate;
        this.bufferSize = bufferSize;

        const source = Harmonia.audioContext.createBufferSource();
        const scriptNode = Harmonia.audioContext.createScriptProcessor(bufferSize, 2, 2);

        let pcmBufferArray: number[][];

        scriptNode.onaudioprocess = (audioProcessingEvent: any) => {
            const outputBuffer = audioProcessingEvent.outputBuffer;
            const numChannels = outputBuffer.numberOfChannels;

            if (pcmBufferArray == null) {
                pcmBufferArray = new Array(numChannels);
                for (let i = 0; i < numChannels; i++) {
                    pcmBufferArray[i] = new Array(bufferSize);
                }
            }

            for (let i = 0; i < numChannels; i++) {
                for (let j = 0; j < bufferSize; j++) {
                    pcmBufferArray[i][j] = 0;
                }
            }

            this.masterGroup.writeRawPCM(pcmBufferArray);

            for (let channel = 0; channel < numChannels; channel++) {
                const outputData = outputBuffer.getChannelData(channel);

                for (let sample = 0; sample < bufferSize; sample++) {
                    outputData[sample] = pcmBufferArray[channel][sample];
                }
            }
        };

        source.connect(scriptNode);
        scriptNode.connect(Harmonia.audioContext.destination);
        source.start();
    }

    public static finalize(): void {
        if (!this.isInitialized) {
            return;
        }

        console.log("Finalize");
        this.isInitialized = false;

    }

    // tslint:disable-next-line:member-ordering
    // tslint:disable-next-line:max-line-length
    public static registerSound(id: string, binaryData: any, loopStartPoint: number = 0, loopLength: number = 0, callback: () => void): void {

        if (this.isRegistrationInProgress) {
            this._setCapturedError(HarmoniaErrorType.REGISTRATION_IN_PROGRESS);
            return;
        }

        this.isRegistrationInProgress = true;

        const exist = this.existsRegisteredBinary(id);

        if (exist) {
            // 登録済みによるエラー
            this._setCapturedError(HarmoniaErrorType.ALREADY_REGISTERED);
            this.isRegistrationInProgress = false;
            if (callback !== null) {
                callback();
            }

            return;
        }

        const registerCompleteCallback = (): void => {
            this.isRegistrationInProgress = false;
        };

        // tslint:disable-next-line:max-line-length
        const data: RegisteredSoundData = new RegisteredSoundData(id, binaryData, loopStartPoint, loopLength, callback, registerCompleteCallback);
        this.registeredSoundDictionary[id] = data;
    }

    // tslint:disable-next-line:max-line-length
    public static registerSounds(idList: string[], binaryDataList: any[], loopStartPointList: number[], loopLengthList: number[], callback: () => void): void {

        let registerCount = 0;
        const maxRegisterCount = idList.length;

        const registerCompleteCallback = (): void => {
            registerCount++;
            this.isRegistrationInProgress = false;

            if (registerCount >= maxRegisterCount) {
                callback();
            } else {
                // tslint:disable-next-line:max-line-length
                this.registerSound(idList[registerCount], binaryDataList[registerCount], loopStartPointList[registerCount], loopLengthList[registerCount], registerCompleteCallback);
            }

        };

        // tslint:disable-next-line:max-line-length
        this.registerSound(idList[registerCount], binaryDataList[registerCount], loopStartPointList[registerCount], loopLengthList[registerCount], registerCompleteCallback);
    }

    public static unregisterSound(id: string): void {
        const exist = this.existsRegisteredBinary(id);

        if (exist) {
            // 停止
            // tslint:disable-next-line:forin
            for (const key in this.playingDataDictionary) {
                const playingData: PlayingData = this.playingDataDictionary[key];
                if (playingData.getRegisteredSoundId() === id) {
                    playingData.stop();

                    this.playingDataDictionary[key] = null;
                }
            }

            // 削除
            this.registeredSoundDictionary[id] = null;
        } else {
            // 未登録によるエラー
            this._setCapturedError(HarmoniaErrorType.UNREGISTERED_SOUND);
            return;
        }
    }

    public static play(registeredId: string, soundId: string = "", targetGroupId: string = ""): PlayingData {
        const isRegistered = this.existsRegisteredBinary(registeredId);

        if (isRegistered) {
            const groupIsExist = this.existsGroup(targetGroupId);

            if (groupIsExist) {

                if (soundId === "") {
                    soundId = new Date().getTime().toString();
                }
                const registeredSound: RegisteredSoundData = this.registeredSoundDictionary[registeredId];
                const playingData: PlayingData = new PlayingData(registeredSound, soundId);

                this.playingDataDictionary[soundId] = playingData;

                if (targetGroupId !== "") {
                    const targetGroup = this.getGroup(targetGroupId);
                    targetGroup.addSound(playingData);
                } else {
                    this.masterGroup.addSound(playingData);
                }

                return playingData;

            } else {
                // グループ未作成エラー
                this._setCapturedError(HarmoniaErrorType.UNCREATED_GROUP);
            }

        } else {
            // バイナリ未登録エラー
            this._setCapturedError(HarmoniaErrorType.UNREGISTERED_SOUND);
        }

        return null;
    }

    public static getPlayingData(soundId: string): PlayingData {
        const exist = this.existsPlayingData(soundId);

        if (!exist) {
            this._setCapturedError(HarmoniaErrorType.NOT_PLAYING);
        }

        const playingData: PlayingData = this.playingDataDictionary[soundId];
        return playingData;
    }

    public static mute(): void {
        this.masterGroup.mute();
    }

    public static unmute(): void {
        this.masterGroup.unmute();
    }

    // tslint:disable-next-line:member-ordering
    public static pause(): void {
        this.audioContext.suspend();
    }

    // tslint:disable-next-line:member-ordering
    public static resume(): void {
        this.audioContext.resume();
    }

    public static stop(): void {
        this.masterGroup.stop();
    }

    // group
    public static createGroup(groupId: string, parentGroupId: string = "", maxPolyphony: number = -1): SoundGroup {
        // 既存チェック
        const exists = this.existsGroup(groupId);

        if (exists) {
            // 既存によるエラー
            this._setCapturedError(HarmoniaErrorType.ALREADY_CREATED_GROUP);
            return null;
        }

        // 生成
        const group = new SoundGroup(groupId, maxPolyphony);
        this.soundGroupDictionary[groupId] = group;

        if (parentGroupId !== "") {
            const parentGroup = this.getGroup(parentGroupId);
            parentGroup.addGroup(group);
        } else {
            this.masterGroup.addGroup(group);
        }

        return group;
    }

    public static getGroup(groupId: string): SoundGroup {
        if (groupId === this.MASTER_GROUP_ID) {
            return this.masterGroup;
        }

        const soundGroup: SoundGroup = this.soundGroupDictionary[groupId];
        return soundGroup;
    }

    public static deleteGroup(groupId: string): void {

        const exist = this.existsGroup(groupId);

        if (!exist) {
            // 未登録によるエラー
            this._setCapturedError(HarmoniaErrorType.UNCREATED_GROUP);
        }

        // グループで再生中のサウンドを削除
        const group: SoundGroup = this.getGroup(groupId);
        group.stop();

        // 削除処理
        const parentGroup: SoundGroup = this.getGroup(group.getParent());
        parentGroup.removeGroup(group);

        this.soundGroupDictionary[groupId] = null;
    }

    public static startCaptureEvents(): void {
        this.needCaptureEvents = true;
    }

    public static getCapturedEvents(): SoundEventData[] {
        if (this.capturedEvents.length === 0) {
            return null;
        }

        const copiedList: SoundEventData[] = this.capturedEvents.concat();
        this.capturedEvents = [];
        return copiedList;
    }

    public static stopCaptureEvents(): void {
        this.needCaptureEvents = false;
        this.capturedEvents = [];
    }

    public static startCaptureErrors(): void {
        this.needCaptureErrors = true;
    }

    public static getCapturedErrors(): HarmoniaErrorData[] {
        if (this.capturedErrors.length === 0) {
            return null;
        }

        const copiedList: HarmoniaErrorData[] = this.capturedErrors.concat();
        this.capturedErrors = [];
        return copiedList;
    }

    public static stopCaptureErrors(): void {
        this.needCaptureErrors = false;
        this.capturedErrors = [];
    }

    public static _removePlayingData(data: PlayingData): void {
        const id: string = data.getId();
        this.playingDataDictionary[id] = null;
    }

    public static _setCapturedEvent(registeredSoundId: string, soundId: string, type: SoundEventType): void {
        if (this.needCaptureEvents) {
            this.capturedEvents.push(new SoundEventData(registeredSoundId, soundId, type));
        }
    }

    public static _setCapturedError(type: HarmoniaErrorType): void {
        if (this.needCaptureErrors) {
            this.capturedErrors.push(new HarmoniaErrorData(type));
        } else {
            throw new Error("Harmonia Error : " + type.toString());
        }
    }

    public static getMasterVolume(): number {
        return this.masterGroup.getVolume();
    }

    public static setMasterVolume(volume: number) {
        this.masterGroup.setVolume(volume);
    }

    public static getSampleRate(): number {
        return this.sampleRate;
    }

    public static getBufferSize(): number {
        return this.bufferSize;
    }

    public static getAudioContext(): AudioContext {
        return this.audioContext;
    }

    private static isInitialized: boolean = false;
    private static MASTER_GROUP_ID: string = "MASTER_GROUP";

    private static audioContext: AudioContext;

    private static isRegistrationInProgress: boolean;

    private static needCaptureEvents: boolean;
    private static needCaptureErrors: boolean;

    private static capturedEvents: SoundEventData[];
    private static capturedErrors: HarmoniaErrorData[];

    private static registeredSoundDictionary: { [id: string]: RegisteredSoundData; };
    private static soundGroupDictionary: { [id: string]: SoundGroup; };
    private static playingDataDictionary: { [id: string]: PlayingData; };

    private static masterGroup: SoundGroup;
    private static sampleRate: number;
    private static bufferSize: number;

    private static existsRegisteredBinary(registeredId: string): boolean {
        const registeredBinary = this.registeredSoundDictionary[registeredId];
        return registeredBinary != null;
    }
    private static existsGroup(groupId: string): boolean {
        if (groupId === "") {
            // master group
            return true;
        }

        const group = this.soundGroupDictionary[groupId];
        return group != null;
    }
    private static existsPlayingData(playingDataId: string): boolean {
        const playingData = this.playingDataDictionary[playingDataId];
        return playingData != null;
    }

}
