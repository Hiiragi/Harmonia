using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;

namespace jp.Hiiragi
{
    public class HarmoniaSoundChannel
    {



#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_sound_pause(string playingDataId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_sound_resume(string playingDataId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_sound_stop(string playingDataId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_sound_mute(string playingDataId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_sound_unmute(string playingDataId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern float harmonia_get_sound_volume(string playingDataId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_set_sound_volume(string playingDataId, float volume);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern uint harmonia_get_sound_status(string playingDataId);


#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern uint harmonia_get_sound_current_position(string playingDataId);




        public uint CurrentPosition
        {
            get
            {
                return harmonia_get_sound_current_position(id);
            }
        }


        public float Volume
        {
            get
            {
                return harmonia_get_sound_volume(id);
            }
            set
            {
                harmonia_set_sound_volume(id, value);
            }
        }


        public string id { get; private set; }








        public HarmoniaSoundChannel(string soundId)
        {
            id = soundId;
        }


        public void Pause()
        {
            harmonia_sound_pause(id);
        }

        public void Resume()
        {
            harmonia_sound_resume(id);
        }


        public void Stop()
        {
            harmonia_sound_stop(id);
        }


        public void Mute()
        {
            harmonia_sound_mute(id);
        }

        public void Unmute()
        {
            harmonia_sound_unmute(id);
        }









    }

}
