class RegisteredSoundData {
    public buffers: Float32Array[];
    public currentPosition: number;
    public loopStartPoint: number;
    public loopLength: number;
    public length: number;
    public numberOfChannels: number;

    private id: string;

    // tslint:disable-next-line:max-line-length
    constructor(id: string, binaryData: any, loopStartPoint: number, loopLength: number, callback: () => void, completeCallback: () => void) {
        this.buffers = [];

        this.id = id;
        this.currentPosition = 0;
        this.loopStartPoint = loopStartPoint;
        this.loopLength = loopLength;

        console.log("start decode");
        Harmonia.getAudioContext().decodeAudioData(binaryData, (decodedBuffer) => {
            console.log("finish decode");

            this.numberOfChannels = decodedBuffer.numberOfChannels;
            this.length = decodedBuffer.length;

            for (let channel = 0; channel < this.numberOfChannels; channel++) {
                this.buffers[channel] = decodedBuffer.getChannelData(channel);
            }

            const ratio = decodedBuffer.sampleRate / 44100;
            this.loopStartPoint = Math.round(this.loopStartPoint * ratio);
            this.loopLength = Math.round(this.loopLength * ratio);

            if (completeCallback != null) {
                completeCallback();
            }

            if (callback != null) {
                callback();
            }

            Harmonia._setCapturedEvent(id, "", SoundEventType.REGISTER_SOUND_COMPLETE);

            });
    }

    public getId(): string {
        return this.id;
    }

}
