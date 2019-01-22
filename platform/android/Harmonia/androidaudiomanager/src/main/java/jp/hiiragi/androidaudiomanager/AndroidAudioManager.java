package jp.hiiragi.androidaudiomanager;

import android.content.Context;
import android.media.AudioManager;
import com.unity3d.player.UnityPlayer;

/**
 * Created by hiiragi on 2017/12/02.
 */

public final class AndroidAudioManager
{
    public static int sampleRate;
    public static int bufferSize;

    public static void initialize()
    {
        /*
        AudioManager am = (AudioManager) UnityPlayer.currentActivity.getSystemService(Context.AUDIO_SERVICE);
        String sampleRateStr = am.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
        sampleRate = Integer.parseInt(sampleRateStr);
        if (sampleRate == 0) sampleRate = 44100;

        String framesPerBuffer = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        bufferSize = Integer.parseInt(framesPerBuffer);
        if (bufferSize == 0) bufferSize = 256; // Use default
        */
    }
}
