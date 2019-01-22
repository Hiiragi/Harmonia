"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
var Harmonia = /** @class */ (function () {
    function Harmonia() {
    }
    // tslint:disable-next-line:member-ordering
    Harmonia.initialize = function (bufferSize) {
        var _this = this;
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
        var AudioContextClass = window.AudioContext || window.webkitAudioContext;
        this.audioContext = new AudioContextClass();
        this.sampleRate = this.audioContext.sampleRate;
        this.bufferSize = bufferSize;
        var source = Harmonia.audioContext.createBufferSource();
        var scriptNode = Harmonia.audioContext.createScriptProcessor(bufferSize, 2, 2);
        var pcmBufferArray;
        scriptNode.onaudioprocess = function (audioProcessingEvent) {
            var outputBuffer = audioProcessingEvent.outputBuffer;
            var numChannels = outputBuffer.numberOfChannels;
            if (pcmBufferArray == null) {
                pcmBufferArray = new Array(numChannels);
                for (var i = 0; i < numChannels; i++) {
                    pcmBufferArray[i] = new Array(bufferSize);
                }
            }
            for (var i = 0; i < numChannels; i++) {
                for (var j = 0; j < bufferSize; j++) {
                    pcmBufferArray[i][j] = 0;
                }
            }
            _this.masterGroup.writeRawPCM(pcmBufferArray);
            for (var channel = 0; channel < numChannels; channel++) {
                var outputData = outputBuffer.getChannelData(channel);
                for (var sample = 0; sample < bufferSize; sample++) {
                    outputData[sample] = pcmBufferArray[channel][sample];
                }
            }
        };
        source.connect(scriptNode);
        scriptNode.connect(Harmonia.audioContext.destination);
        source.start();
    };
    Harmonia.finalize = function () {
        if (!this.isInitialized) {
            return;
        }
        console.log("Finalize");
        this.isInitialized = false;
    };
    // tslint:disable-next-line:member-ordering
    // tslint:disable-next-line:max-line-length
    Harmonia.registerSound = function (id, binaryData, loopStartPoint, loopLength, callback) {
        var _this = this;
        if (loopStartPoint === void 0) { loopStartPoint = 0; }
        if (loopLength === void 0) { loopLength = 0; }
        if (this.isRegistrationInProgress) {
            this._setCapturedError(HarmoniaErrorType.REGISTRATION_IN_PROGRESS);
            return;
        }
        this.isRegistrationInProgress = true;
        var exist = this.existsRegisteredBinary(id);
        if (exist) {
            // 登録済みによるエラー
            this._setCapturedError(HarmoniaErrorType.ALREADY_REGISTERED);
            this.isRegistrationInProgress = false;
            if (callback !== null) {
                callback();
            }
            return;
        }
        var registerCompleteCallback = function () {
            _this.isRegistrationInProgress = false;
        };
        // tslint:disable-next-line:max-line-length
        var data = new RegisteredSoundData(id, binaryData, loopStartPoint, loopLength, callback, registerCompleteCallback);
        this.registeredSoundDictionary[id] = data;
    };
    // tslint:disable-next-line:max-line-length
    Harmonia.registerSounds = function (idList, binaryDataList, loopStartPointList, loopLengthList, callback) {
        var _this = this;
        var registerCount = 0;
        var maxRegisterCount = idList.length;
        var registerCompleteCallback = function () {
            registerCount++;
            _this.isRegistrationInProgress = false;
            if (registerCount >= maxRegisterCount) {
                callback();
            }
            else {
                // tslint:disable-next-line:max-line-length
                _this.registerSound(idList[registerCount], binaryDataList[registerCount], loopStartPointList[registerCount], loopLengthList[registerCount], registerCompleteCallback);
            }
        };
        // tslint:disable-next-line:max-line-length
        this.registerSound(idList[registerCount], binaryDataList[registerCount], loopStartPointList[registerCount], loopLengthList[registerCount], registerCompleteCallback);
    };
    Harmonia.unregisterSound = function (id) {
        var exist = this.existsRegisteredBinary(id);
        if (exist) {
            // 停止
            // tslint:disable-next-line:forin
            for (var key in this.playingDataDictionary) {
                var playingData = this.playingDataDictionary[key];
                if (playingData.getRegisteredSoundId() === id) {
                    playingData.stop();
                    this.playingDataDictionary[key] = null;
                }
            }
            // 削除
            this.registeredSoundDictionary[id] = null;
        }
        else {
            // 未登録によるエラー
            this._setCapturedError(HarmoniaErrorType.UNREGISTERED_SOUND);
            return;
        }
    };
    Harmonia.play = function (registeredId, soundId, targetGroupId) {
        if (soundId === void 0) { soundId = ""; }
        if (targetGroupId === void 0) { targetGroupId = ""; }
        var isRegistered = this.existsRegisteredBinary(registeredId);
        if (isRegistered) {
            var groupIsExist = this.existsGroup(targetGroupId);
            if (groupIsExist) {
                if (soundId === "") {
                    soundId = new Date().getTime().toString();
                }
                var registeredSound = this.registeredSoundDictionary[registeredId];
                var playingData = new PlayingData(registeredSound, soundId);
                this.playingDataDictionary[soundId] = playingData;
                if (targetGroupId !== "") {
                    var targetGroup = this.getGroup(targetGroupId);
                    targetGroup.addSound(playingData);
                }
                else {
                    this.masterGroup.addSound(playingData);
                }
                return playingData;
            }
            else {
                // グループ未作成エラー
                this._setCapturedError(HarmoniaErrorType.UNCREATED_GROUP);
            }
        }
        else {
            // バイナリ未登録エラー
            this._setCapturedError(HarmoniaErrorType.UNREGISTERED_SOUND);
        }
        return null;
    };
    Harmonia.getPlayingData = function (soundId) {
        var exist = this.existsPlayingData(soundId);
        if (!exist) {
            this._setCapturedError(HarmoniaErrorType.NOT_PLAYING);
        }
        var playingData = this.playingDataDictionary[soundId];
        return playingData;
    };
    Harmonia.mute = function () {
        this.masterGroup.mute();
    };
    Harmonia.unmute = function () {
        this.masterGroup.unmute();
    };
    // tslint:disable-next-line:member-ordering
    Harmonia.pause = function () {
        this.audioContext.suspend();
    };
    // tslint:disable-next-line:member-ordering
    Harmonia.resume = function () {
        this.audioContext.resume();
    };
    Harmonia.stop = function () {
        this.masterGroup.stop();
    };
    // group
    Harmonia.createGroup = function (groupId, parentGroupId, maxPolyphony) {
        if (parentGroupId === void 0) { parentGroupId = ""; }
        if (maxPolyphony === void 0) { maxPolyphony = -1; }
        // 既存チェック
        var exists = this.existsGroup(groupId);
        if (exists) {
            // 既存によるエラー
            this._setCapturedError(HarmoniaErrorType.ALREADY_CREATED_GROUP);
            return null;
        }
        // 生成
        var group = new SoundGroup(groupId, maxPolyphony);
        this.soundGroupDictionary[groupId] = group;
        if (parentGroupId !== "") {
            var parentGroup = this.getGroup(parentGroupId);
            parentGroup.addGroup(group);
        }
        else {
            this.masterGroup.addGroup(group);
        }
        return group;
    };
    Harmonia.getGroup = function (groupId) {
        if (groupId === this.MASTER_GROUP_ID) {
            return this.masterGroup;
        }
        var soundGroup = this.soundGroupDictionary[groupId];
        return soundGroup;
    };
    Harmonia.deleteGroup = function (groupId) {
        var exist = this.existsGroup(groupId);
        if (!exist) {
            // 未登録によるエラー
            this._setCapturedError(HarmoniaErrorType.UNCREATED_GROUP);
        }
        // グループで再生中のサウンドを削除
        var group = this.getGroup(groupId);
        group.stop();
        // 削除処理
        var parentGroup = this.getGroup(group.getParent());
        parentGroup.removeGroup(group);
        this.soundGroupDictionary[groupId] = null;
    };
    Harmonia.startCaptureEvents = function () {
        this.needCaptureEvents = true;
    };
    Harmonia.getCapturedEvents = function () {
        if (this.capturedEvents.length === 0) {
            return null;
        }
        var copiedList = this.capturedEvents.concat();
        this.capturedEvents = [];
        return copiedList;
    };
    Harmonia.stopCaptureEvents = function () {
        this.needCaptureEvents = false;
        this.capturedEvents = [];
    };
    Harmonia.startCaptureErrors = function () {
        this.needCaptureErrors = true;
    };
    Harmonia.getCapturedErrors = function () {
        if (this.capturedErrors.length === 0) {
            return null;
        }
        var copiedList = this.capturedErrors.concat();
        this.capturedErrors = [];
        return copiedList;
    };
    Harmonia.stopCaptureErrors = function () {
        this.needCaptureErrors = false;
        this.capturedErrors = [];
    };
    Harmonia._removePlayingData = function (data) {
        var id = data.getId();
        this.playingDataDictionary[id] = null;
    };
    Harmonia._setCapturedEvent = function (registeredSoundId, soundId, type) {
        if (this.needCaptureEvents) {
            this.capturedEvents.push(new SoundEventData(registeredSoundId, soundId, type));
        }
    };
    Harmonia._setCapturedError = function (type) {
        if (this.needCaptureErrors) {
            this.capturedErrors.push(new HarmoniaErrorData(type));
        }
        else {
            throw new Error("Harmonia Error : " + type.toString());
        }
    };
    Harmonia.getMasterVolume = function () {
        return this.masterGroup.getVolume();
    };
    Harmonia.setMasterVolume = function (volume) {
        this.masterGroup.setVolume(volume);
    };
    Harmonia.getSampleRate = function () {
        return this.sampleRate;
    };
    Harmonia.getBufferSize = function () {
        return this.bufferSize;
    };
    Harmonia.getAudioContext = function () {
        return this.audioContext;
    };
    Harmonia.existsRegisteredBinary = function (registeredId) {
        var registeredBinary = this.registeredSoundDictionary[registeredId];
        return registeredBinary != null;
    };
    Harmonia.existsGroup = function (groupId) {
        if (groupId === "") {
            // master group
            return true;
        }
        var group = this.soundGroupDictionary[groupId];
        return group != null;
    };
    Harmonia.existsPlayingData = function (playingDataId) {
        var playingData = this.playingDataDictionary[playingDataId];
        return playingData != null;
    };
    Harmonia.isInitialized = false;
    Harmonia.MASTER_GROUP_ID = "MASTER_GROUP";
    return Harmonia;
}());
var AbstractSoundData = /** @class */ (function () {
    function AbstractSoundData(id) {
        this.id = id;
        this.isMuted = false;
        this.currentStatus = SoundStatus.PLAYING;
        this.currentVolume = 1.0;
        this.mutedGroupList = [];
        this.pausedGroupList = [];
        this.effectCommandList = [];
    }
    AbstractSoundData.prototype.getId = function () {
        return this.id;
    };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype.writeRawPCM = function (pcmBufferArray) { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype.pause = function () { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype.resume = function () { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype.stop = function () { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype.mute = function () { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype.unmute = function () { };
    AbstractSoundData.prototype.getVolume = function () {
        return this.currentVolume;
    };
    AbstractSoundData.prototype.setVolume = function (volume) {
        this.currentVolume = volume;
    };
    AbstractSoundData.prototype.getParent = function () {
        return this.parentGroupId;
    };
    AbstractSoundData.prototype.setParent = function (parentGroupId) {
        this.parentGroupId = parentGroupId;
    };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype._pauseByGroup = function (soundGroupId) { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype._resumeByGroup = function (soundGroupId) { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype._muteByGroup = function (soundGroupId) { };
    // tslint:disable-next-line:no-empty
    AbstractSoundData.prototype._unmuteByGroup = function (soundGroupId) { };
    return AbstractSoundData;
}());
var PlayingData = /** @class */ (function (_super) {
    __extends(PlayingData, _super);
    function PlayingData(registerSoundData, soundId) {
        var _this = _super.call(this, soundId) || this;
        _this.registerSoundData = registerSoundData;
        _this.currentPosition = 0;
        return _this;
    }
    PlayingData.prototype.writeRawPCM = function (pcmBufferArray) {
        // 再生中以外、あるいはグループによって一時停止されている場合は、何もせずに終了
        if (this.currentStatus !== SoundStatus.PLAYING || this.pausedGroupList.length > 0) {
            return;
        }
        var numChannels = pcmBufferArray.length;
        var bufferSize = pcmBufferArray[0].length;
        if (this.bufferArray == null) {
            this.bufferArray = new Array(numChannels);
            for (var i = 0; i < numChannels; i++) {
                this.bufferArray[i] = new Array(bufferSize);
            }
        }
        for (var channel = 0; channel < numChannels; channel++) {
            var channelData = void 0;
            // mono/stereo 対応
            if (this.registerSoundData.numberOfChannels === 1) {
                channelData = this.getChannelData(0, this.currentPosition, bufferSize);
            }
            else {
                channelData = this.getChannelData(channel, this.currentPosition, bufferSize);
            }
            for (var sample = 0; sample < bufferSize; sample++) {
                this.bufferArray[channel][sample] = channelData[sample] * this.currentVolume;
            }
        }
        // エフェクトコマンド実行
        var effectCommandLength = this.effectCommandList.length;
        if (effectCommandLength > 0) {
            for (var i = 0; i < effectCommandLength; i++) {
                this.effectCommandList[i].execute(this.bufferArray);
            }
        }
        // 合算
        for (var channel = 0; channel < numChannels; channel++) {
            var channelData = this.bufferArray[channel];
            for (var sample = 0; sample < bufferSize; sample++) {
                pcmBufferArray[channel][sample] += channelData[sample];
            }
        }
        if (this.playingStatus === PlayingStatus.NORMAL) {
            this.currentPosition += bufferSize;
        }
        else if (this.playingStatus === PlayingStatus.LOOPED) {
            this.currentPosition = this.playingTimeByLoop;
        }
        else if (this.playingStatus === PlayingStatus.END) {
            this.currentPosition = this.registerSoundData.length;
            this.stop();
            Harmonia._setCapturedEvent(this.registerSoundData.getId(), this.id, SoundEventType.SOUND_PLAY_COMPLETE);
        }
        // console.log(this.currentPosition);
    };
    PlayingData.prototype.pause = function () {
        console.log("pause() / id : " + this.id);
        if (this.currentStatus === SoundStatus.PLAYING) {
            this.currentStatus = SoundStatus.PAUSED;
        }
    };
    PlayingData.prototype.resume = function () {
        console.log("resume() / id : " + this.id);
        if (this.currentStatus === SoundStatus.PAUSED) {
            this.currentStatus = SoundStatus.PLAYING;
        }
    };
    PlayingData.prototype.stop = function () {
        console.log("stop() / id : " + this.id);
        this.currentStatus = SoundStatus.STOPPED;
        // 削除
        var parentGroup = Harmonia.getGroup(this.parentGroupId);
        parentGroup._registerStoppedSound(this);
    };
    PlayingData.prototype.mute = function () {
        console.log("mute() / id : " + this.id);
        this.isMuted = true;
    };
    PlayingData.prototype.unmute = function () {
        console.log("unmute() / id : " + this.id);
        this.isMuted = false;
    };
    PlayingData.prototype.destroy = function () {
        Harmonia._removePlayingData(this);
        var parentGroup = Harmonia.getGroup(this.parentGroupId);
        parentGroup.removeSound(this);
    };
    PlayingData.prototype.getCurrentStatus = function () {
        return this.currentStatus;
    };
    PlayingData.prototype.getCurrentPosition = function () {
        return this.currentPosition;
    };
    PlayingData.prototype.getRegisteredSoundId = function () {
        return this.registerSoundData.getId();
    };
    PlayingData.prototype._pauseByGroup = function (soundGroupId) {
        this.pausedGroupList.push(soundGroupId);
    };
    PlayingData.prototype._resumeByGroup = function (soundGroupId) {
        var index = this.pausedGroupList.indexOf(soundGroupId);
        if (index >= 0) {
            this.pausedGroupList.splice(index, 1);
        }
    };
    PlayingData.prototype._muteByGroup = function (soundGroupId) {
        this.mutedGroupList.push(soundGroupId);
    };
    PlayingData.prototype._unmuteByGroup = function (soundGroupId) {
        var index = this.mutedGroupList.indexOf(soundGroupId);
        if (index >= 0) {
            this.mutedGroupList.splice(index, 1);
        }
    };
    PlayingData.prototype.getChannelData = function (channel, position, size) {
        var source = this.registerSoundData.buffers[channel];
        if (this.slicedArray != null) {
            this.slicedArray.fill(0);
        }
        else {
            this.slicedArray = new Float32Array(size);
        }
        var len;
        var loopStartPoint = this.registerSoundData.loopStartPoint;
        var loopLength = this.registerSoundData.loopLength;
        var length = this.registerSoundData.length;
        var isMuted = this.isMuted || this.mutedGroupList.length > 0;
        if (loopLength > 0 && position + size > loopStartPoint + loopLength) {
            // ループありで、ループポイントからはみ出る
            // console.log("ループありで、ループポイントからはみ出る");
            var i = 0;
            if (!isMuted) {
                len = loopStartPoint + loopLength - position;
                for (i; i < len; i++) {
                    this.slicedArray[i] = source[position + i] * this.currentVolume;
                }
            }
            var j = 0;
            if (!isMuted) {
                len = size - len;
                for (j; j < len; j++) {
                    this.slicedArray[i++] = source[loopStartPoint + j] * this.currentVolume;
                }
            }
            this.playingTimeByLoop = loopStartPoint + j;
            this.playingStatus = PlayingStatus.LOOPED;
        }
        else if (position + size > length) {
            if (position <= length) {
                // 後ろにはみ出る
                // console.log("後ろにはみ出る");
                if (!isMuted) {
                    len = length - position;
                    for (var i = 0; i < len; i++) {
                        this.slicedArray[i] = source[position + i] * this.currentVolume;
                    }
                }
                this.playingStatus = PlayingStatus.END;
            }
        }
        else {
            // それ以外
            if (!isMuted) {
                len = this.slicedArray.length;
                for (var i = 0; i < len; i++) {
                    this.slicedArray[i] = source[position + i] * this.currentVolume;
                }
            }
            this.playingStatus = PlayingStatus.NORMAL;
        }
        return this.slicedArray;
    };
    return PlayingData;
}(AbstractSoundData));
var PlayingStatus;
(function (PlayingStatus) {
    PlayingStatus[PlayingStatus["NORMAL"] = 0] = "NORMAL";
    PlayingStatus[PlayingStatus["LOOPED"] = 1] = "LOOPED";
    PlayingStatus[PlayingStatus["END"] = 2] = "END";
})(PlayingStatus || (PlayingStatus = {}));
var RegisteredSoundData = /** @class */ (function () {
    // tslint:disable-next-line:max-line-length
    function RegisteredSoundData(id, binaryData, loopStartPoint, loopLength, callback, completeCallback) {
        var _this = this;
        this.buffers = [];
        this.id = id;
        this.currentPosition = 0;
        this.loopStartPoint = loopStartPoint;
        this.loopLength = loopLength;
        console.log("start decode");
        Harmonia.getAudioContext().decodeAudioData(binaryData, function (decodedBuffer) {
            console.log("finish decode");
            _this.numberOfChannels = decodedBuffer.numberOfChannels;
            _this.length = decodedBuffer.length;
            for (var channel = 0; channel < _this.numberOfChannels; channel++) {
                _this.buffers[channel] = decodedBuffer.getChannelData(channel);
            }
            var ratio = decodedBuffer.sampleRate / 44100;
            _this.loopStartPoint = Math.round(_this.loopStartPoint * ratio);
            _this.loopLength = Math.round(_this.loopLength * ratio);
            if (completeCallback != null) {
                completeCallback();
            }
            if (callback != null) {
                callback();
            }
            Harmonia._setCapturedEvent(id, "", SoundEventType.REGISTER_SOUND_COMPLETE);
        });
    }
    RegisteredSoundData.prototype.getId = function () {
        return this.id;
    };
    return RegisteredSoundData;
}());
var SoundGroup = /** @class */ (function (_super) {
    __extends(SoundGroup, _super);
    function SoundGroup(id, maxPolyphony) {
        if (maxPolyphony === void 0) { maxPolyphony = 0; }
        var _this = _super.call(this, id) || this;
        _this.soundDataList = [];
        _this.stoppedSoundDataList = [];
        _this.maxPolyphony = maxPolyphony;
        _this.currentPolyphony = 0;
        return _this;
    }
    SoundGroup.prototype.addSound = function (sound) {
        if (this.maxPolyphony > 0 && this.currentPolyphony === this.maxPolyphony) {
            return;
        }
        this.currentPolyphony++;
        this.addData(sound);
    };
    SoundGroup.prototype.addGroup = function (group) {
        this.addData(group);
    };
    SoundGroup.prototype.removeSound = function (sound) {
        this.currentPolyphony--;
        this.removeData(sound);
    };
    SoundGroup.prototype.removeGroup = function (group) {
        this.removeData(group);
    };
    SoundGroup.prototype.writeRawPCM = function (pcmBufferArray) {
        // 再生中以外、あるいはグループによって一時停止されている場合は、何もせずに終了
        if (this.currentStatus !== SoundStatus.PLAYING || this.pausedGroupList.length > 0) {
            return;
        }
        // 停止しているものは削除
        var size = this.stoppedSoundDataList.length;
        if (size > 0) {
            for (var i = 0; i < size; i++) {
                this.stoppedSoundDataList[i].destroy();
            }
            this.stoppedSoundDataList.length = 0;
        }
        var numChannels = pcmBufferArray.length;
        var bufferSize = pcmBufferArray[0].length;
        if (this.tempBufferList == null) {
            this.tempBufferList = new Array(numChannels);
            for (var i = 0; i < numChannels; i++) {
                this.tempBufferList[i] = new Array(bufferSize);
            }
        }
        // リセット
        for (var i = 0; i < numChannels; i++) {
            for (var j = 0; j < bufferSize; j++) {
                this.tempBufferList[i][j] = 0;
            }
        }
        // 書き込み
        for (var _i = 0, _a = this.soundDataList; _i < _a.length; _i++) {
            var data = _a[_i];
            data.writeRawPCM(this.tempBufferList);
        }
        // ボリューム反映
        for (var channel = 0; channel < numChannels; channel++) {
            var buffer = this.tempBufferList[channel];
            for (var sample = 0; sample < bufferSize; sample++) {
                buffer[sample] *= this.currentVolume;
            }
        }
        // エフェクトコマンド実行
        size = this.effectCommandList.length;
        if (size > 0) {
            for (var i = 0; i < size; i++) {
                this.effectCommandList[i].execute(this.tempBufferList);
            }
        }
        // 合算
        for (var channel = 0; channel < numChannels; channel++) {
            var outputData = pcmBufferArray[channel];
            var wroteData = this.tempBufferList[channel];
            for (var sample = 0; sample < bufferSize; sample++) {
                outputData[sample] += wroteData[sample];
            }
        }
    };
    SoundGroup.prototype.pause = function () {
        if (this.currentStatus !== SoundStatus.PLAYING) {
            return;
        }
        this.currentStatus = SoundStatus.PAUSED;
        this._pauseByGroup(this.id);
    };
    SoundGroup.prototype.resume = function () {
        if (this.currentStatus !== SoundStatus.PAUSED) {
            return;
        }
        this.currentStatus = SoundStatus.PLAYING;
        this._resumeByGroup(this.id);
    };
    SoundGroup.prototype.stop = function () {
        var size = this.soundDataList.length;
        if (size > 0) {
            for (var i = 0; i < size; i++) {
                this.soundDataList[i].stop();
            }
        }
    };
    SoundGroup.prototype.mute = function () {
        if (this.isMuted) {
            return;
        }
        this.isMuted = true;
        this._muteByGroup(this.id);
    };
    SoundGroup.prototype.unmute = function () {
        if (!this.isMuted) {
            return;
        }
        this.isMuted = false;
        this._unmuteByGroup(this.id);
    };
    SoundGroup.prototype.getCurrentPolyphony = function () {
        return this.currentPolyphony;
    };
    SoundGroup.prototype.setDucker = function (triggerGroup, ratio, attackTimeByMS, releaseTimeByMS) {
        if (attackTimeByMS === void 0) { attackTimeByMS = 0; }
        if (releaseTimeByMS === void 0) { releaseTimeByMS = 0; }
        this.effectCommandList.push(new DuckerCommand(triggerGroup, ratio, attackTimeByMS, releaseTimeByMS));
    };
    SoundGroup.prototype._pauseByGroup = function (soundGroupId) {
        var size = this.soundDataList.length;
        for (var i = 0; i < size; i++) {
            this.soundDataList[i]._pauseByGroup(soundGroupId);
        }
    };
    SoundGroup.prototype._resumeByGroup = function (soundGroupId) {
        var size = this.soundDataList.length;
        for (var i = 0; i < size; i++) {
            this.soundDataList[i]._resumeByGroup(soundGroupId);
        }
    };
    SoundGroup.prototype._muteByGroup = function (soundGroupId) {
        var size = this.soundDataList.length;
        for (var i = 0; i < size; i++) {
            this.soundDataList[i]._muteByGroup(soundGroupId);
        }
    };
    SoundGroup.prototype._unmuteByGroup = function (soundGroupId) {
        var size = this.soundDataList.length;
        for (var i = 0; i < size; i++) {
            this.soundDataList[i]._unmuteByGroup(soundGroupId);
        }
    };
    SoundGroup.prototype._registerStoppedSound = function (playingData) {
        this.stoppedSoundDataList.push(playingData);
    };
    SoundGroup.prototype.addData = function (data) {
        this.soundDataList.push(data);
        data.setParent(this.id);
        // ミュート状態、ポーズ状態を継承
        if (this.mutedGroupList.length > 0) {
            var size = this.mutedGroupList.length;
            for (var i = 0; size; i++) {
                data._muteByGroup(this.mutedGroupList[i]);
            }
        }
        if (this.pausedGroupList.length > 0) {
            var size = this.pausedGroupList.length;
            for (var i = 0; size; i++) {
                data._pauseByGroup(this.pausedGroupList[i]);
            }
        }
        if (this.isMuted) {
            data._muteByGroup(this.id);
        }
        if (this.currentStatus === SoundStatus.PAUSED) {
            data._pauseByGroup(this.id);
        }
    };
    SoundGroup.prototype.removeData = function (data) {
        var index = this.soundDataList.indexOf(data);
        this.soundDataList.splice(index, 1);
        data.setParent(null);
    };
    return SoundGroup;
}(AbstractSoundData));
var SoundEventData = /** @class */ (function () {
    function SoundEventData(registeredSoundId, soundId, type) {
        this.rid = registeredSoundId;
        this.sid = soundId;
        this.type = type;
    }
    SoundEventData.prototype.getRid = function () {
        return this.rid;
    };
    SoundEventData.prototype.getSid = function () {
        return this.sid;
    };
    SoundEventData.prototype.getType = function () {
        return this.type;
    };
    return SoundEventData;
}());
var HarmoniaErrorData = /** @class */ (function () {
    function HarmoniaErrorData(type) {
        this.type = type;
    }
    HarmoniaErrorData.prototype.getErrorType = function () {
        return this.type;
    };
    return HarmoniaErrorData;
}());
var SoundEventType;
(function (SoundEventType) {
    SoundEventType[SoundEventType["REGISTER_SOUND_COMPLETE"] = 0] = "REGISTER_SOUND_COMPLETE";
    SoundEventType[SoundEventType["SOUND_PLAY_COMPLETE"] = 1] = "SOUND_PLAY_COMPLETE";
})(SoundEventType || (SoundEventType = {}));
var SoundStatus;
(function (SoundStatus) {
    SoundStatus[SoundStatus["WAIT_INITIALIZE"] = 0] = "WAIT_INITIALIZE";
    SoundStatus[SoundStatus["PLAYING"] = 1] = "PLAYING";
    SoundStatus[SoundStatus["PAUSED"] = 2] = "PAUSED";
    SoundStatus[SoundStatus["STOPPED"] = 3] = "STOPPED";
})(SoundStatus || (SoundStatus = {}));
var HarmoniaErrorType;
(function (HarmoniaErrorType) {
    HarmoniaErrorType[HarmoniaErrorType["ALREADY_REGISTERED"] = 0] = "ALREADY_REGISTERED";
    HarmoniaErrorType[HarmoniaErrorType["UNREGISTERED_SOUND"] = 1] = "UNREGISTERED_SOUND";
    HarmoniaErrorType[HarmoniaErrorType["ALREADY_CREATED_GROUP"] = 2] = "ALREADY_CREATED_GROUP";
    HarmoniaErrorType[HarmoniaErrorType["UNCREATED_GROUP"] = 3] = "UNCREATED_GROUP";
    HarmoniaErrorType[HarmoniaErrorType["NOT_PLAYING"] = 4] = "NOT_PLAYING";
    HarmoniaErrorType[HarmoniaErrorType["REGISTRATION_IN_PROGRESS"] = 1000] = "REGISTRATION_IN_PROGRESS";
})(HarmoniaErrorType || (HarmoniaErrorType = {}));
var AbstractEffectCommand = /** @class */ (function () {
    function AbstractEffectCommand() {
    }
    // tslint:disable-next-line:no-empty
    AbstractEffectCommand.prototype.execute = function (pcmBufferArray) { };
    return AbstractEffectCommand;
}());
var DuckerCommand = /** @class */ (function (_super) {
    __extends(DuckerCommand, _super);
    function DuckerCommand(triggerGroup, ratio, attackTimeByMS, releaseTimeByMS) {
        if (attackTimeByMS === void 0) { attackTimeByMS = 0; }
        if (releaseTimeByMS === void 0) { releaseTimeByMS = 0; }
        var _this = _super.call(this) || this;
        _this.triggerGroup = triggerGroup;
        _this.ratio = ratio;
        _this.currentRatio = 1;
        _this.diffRatioForAttackTimeBySample = (1.0 - ratio) / (44100 * attackTimeByMS / 1000);
        _this.diffRatioForReleaseTimeBySample = (1.0 - ratio) / (44100 * releaseTimeByMS / 1000);
        return _this;
    }
    DuckerCommand.prototype.execute = function (pcmBufferArray) {
        var numOfChannels = pcmBufferArray.length;
        for (var channel = 0; channel < numOfChannels; channel++) {
            var output = pcmBufferArray[channel];
            var size = output.length;
            if (this.triggerGroup.getCurrentPolyphony() > 0) {
                for (var i = 0; i < size; i++) {
                    if (this.currentRatio > this.ratio) {
                        this.currentRatio -= this.diffRatioForAttackTimeBySample;
                        if (this.currentRatio < this.ratio) {
                            this.currentRatio = this.ratio;
                        }
                    }
                    output[i] *= this.currentRatio;
                }
            }
            else {
                for (var i = 0; i < size; i++) {
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
    };
    return DuckerCommand;
}(AbstractEffectCommand));
