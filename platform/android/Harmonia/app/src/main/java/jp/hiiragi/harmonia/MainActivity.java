package jp.hiiragi.harmonia;

import android.content.Context;
import android.media.AudioManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("Harmonia");
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d("hoge","onPause");
        harmonia_pause_all();
    }


    @Override
    public void onResume() {
        super.onResume();
        Log.d("hoge","onResume");
        harmonia_resume_all();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        /*
        AudioManager am = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        String framesPerBuffer = am.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        int bufferSize = Integer.parseInt(framesPerBuffer);
        if (bufferSize == 0) bufferSize = 256; // Use default
        */
        harmonia_initialize(2048);




        int id = this.getResources().getIdentifier("test", "raw", this.getPackageName());
        if (id == 0)
        {    //エラーにはならない
            try {
                throw new FileNotFoundException();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            }
        }
        InputStream is = this.getResources().openRawResource(id);
        byte[] cachedByte = new byte[1];    //read() 毎に読み込むバッファ
        BufferedInputStream bis = new BufferedInputStream(is, 1);
        List<Byte> byteArray = new ArrayList();

        try {
            while ( bis.read(cachedByte, 0, 1) > 0)
            {
                byteArray.add(cachedByte[0]);
            }

            int byteSize = byteArray.size();
            byte[] bytes = new byte[byteSize];


            for (int i = 0; i < byteSize; i++)
            {
                byte a = byteArray.get(i);
                bytes[i] = a;
            }

            harmonia_register_sound("bgm1", bytes, bytes.length, 269128, 2116816);
            harmonia_sound_play("bgm1", "bgm", "");
        }
        catch (IOException e)
        {

        }
        finally
        {
            try {
                bis.close();            //ストリームを閉じる
            } catch (Exception e) {
                //IOException
            }
        }


    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
// General
    public native void harmonia_initialize(int bufferSize);
    public native void harmonia_finalize();

    public native void harmonia_register_sound(String id, byte[] binaryData, int size, int loopStartPoint, int loopLength);
    public native void harmonia_unregister_sound(String id);
    public native void harmonia_pause_all();
    public native void harmonia_resume_all();
    public native void harmonia_stop_all();

    public native float harmonia_get_master_volume();
    public native void harmonia_set_master_volume(float volume);

    public native void harmonia_mute_all();
    public native void harmonia_unmute_all();
/*
    public native void harmonia_start_capture_events();
    public native String harmonia_get_capture_events();
    public native void harmonia_stop_capture_events();

    public native void harmonia_start_capture_errors();
    public native String harmonia_get_capture_errors();
    public native void harmonia_stop_capture_errors();
*/
    // Sound
    public native void harmonia_sound_play(String registeredId, String soundId, String targetGroupId);
    /*
    public native void harmonia_sound_pause(String playingDataId);
    public native void harmonia_sound_resume(String playingDataId);
    public native void harmonia_sound_stop(String playingDataId);
    public native void harmonia_sound_mute(String playingDataId);
    public native void harmonia_sound_unmute(String playingDataId);
    public native float harmonia_get_sound_volume(String playingDataId);
    public native void harmonia_set_sound_volume(String playingDataId, float volume);
    public native int harmonia_get_sound_status(String playingDataId);

    // Group
    public native void harmonia_create_group(String groupId, String parentGroupId, int maxPolyphony);
    public native void harmonia_delete_group(String groupId);
    public native void harmonia_group_pause(String groupId);
    public native void harmonia_group_resume(String groupId);
    public native void harmonia_group_stop(String groupId);
    public native void harmonia_group_mute(String groupId);
    public native void harmonia_group_unmute(String groupId);
    public native float harmonia_get_group_volume(String groupId);
    public native void harmonia_set_group_volume(String groupId, float volume);
    public native void harmonia_set_ducker(String triggerGroupId, String targetGroupId, float ratio);
    public native int harmonia_get_group_status(String targetGroupId);
*/

}
