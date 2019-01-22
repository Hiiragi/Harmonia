class DuckerCommand extends AbstractEffectCommand {

    private triggerGroup: SoundGroup;
    private ratio: number;
    private currentRatio: number;
    private diffRatioForAttackTimeBySample: number;
    private diffRatioForReleaseTimeBySample: number;

    constructor(triggerGroup: SoundGroup, ratio: number, attackTimeByMS: number = 0, releaseTimeByMS: number = 0) {
        super();
        this.triggerGroup = triggerGroup;
        this.ratio = ratio;
        this.currentRatio = 1;
        this.diffRatioForAttackTimeBySample = (1.0 - ratio) / (44100 * attackTimeByMS / 1000);
        this.diffRatioForReleaseTimeBySample = (1.0 - ratio) / (44100 * releaseTimeByMS / 1000);
    }

    public execute(pcmBufferArray: number[][]) {
        const numOfChannels: number = pcmBufferArray.length;
        for (let channel = 0; channel < numOfChannels; channel++) {
            const output = pcmBufferArray[channel];
            const size = output.length;

            if (this.triggerGroup.getCurrentPolyphony() > 0) {
                for (let i = 0; i < size; i++) {

                    if (this.currentRatio > this.ratio) {
                        this.currentRatio -= this.diffRatioForAttackTimeBySample;
                        if (this.currentRatio < this.ratio) {
                            this.currentRatio = this.ratio;
                        }
                    }
                    output[i] *= this.currentRatio;
                }
            } else {
                for (let i = 0; i < size; i++) {

                    if (this.currentRatio < 1) {
                        this.currentRatio += this.diffRatioForReleaseTimeBySample;
                        if (this.currentRatio > 1) {
                            this.currentRatio = 1;
                        }
                    }
                    output[i] *= this.currentRatio;
                }
            }

        }
    }
}
