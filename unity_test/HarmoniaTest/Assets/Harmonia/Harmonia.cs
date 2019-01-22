using AOT;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;
using UnityEngine.Events;

namespace jp.Hiiragi
{
    public class Harmonia
    {

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_initialize(uint bufferSize);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_finalize();


        
#if (UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
    [DllImport("Harmonia")]
#endif
#if UNITY_WEBGL && !UNITY_EDITOR
    private static extern void harmonia_register_sound(string id, byte[] binaryData, uint size, uint loopStartPoint = 0, uint loopLength = 0, Action callback = null);
#else
    private static extern void harmonia_register_sound(string id, byte[] binaryData, uint size, uint loopStartPoint = 0, uint loopLength = 0);
#endif
        

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_unregister_sound(string id);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_pause_all();
#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_resume_all();
#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_stop_all();

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern float harmonia_get_master_volume();
#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_set_master_volume(float volume);
#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_mute_all();
#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_unmute_all();

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_start_capture_events();

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern string harmonia_get_capture_events();

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern IntPtr harmonia_get_capture_events_with_size(out int size);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_stop_capture_events();

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_start_capture_errors();


#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern string harmonia_get_capture_errors();


#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern IntPtr harmonia_get_capture_errors_with_size(out int size);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_stop_capture_errors();


#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_sound_play(string registeredId, string soundId = "", string targetGruopId = "");







        static bool isLoading;
        static Action _callbackForRegister;
        static int _registeredSize;
        static int _registerSize;




        public static float MasterVolume
        {
            get
            {
                return harmonia_get_master_volume();
            }
            set
            {
                harmonia_set_master_volume(value);
            }
        }





        public static void Initialize(uint bufferSize)
        {
            harmonia_initialize(bufferSize);
        }

        public static void Finaliz()
        {
            harmonia_finalize();
        }

        
        public static void RegisterSound(string id, byte[] binaryData, uint size, uint loopStartPoint = 0, uint loopLength = 0, Action callback = null)
        {
            if(isLoading)
                return;

            isLoading = true;

#if UNITY_WEBGL && !UNITY_EDITOR
            harmonia_register_sound(id, binaryData, size, loopStartPoint, loopLength, _CallbackForRegisterComplete);
#else
            harmonia_register_sound(id, binaryData, size, loopStartPoint, loopLength);
#endif
            // コールバック処理
            if(callback != null)
            {

#if UNITY_WEBGL && !UNITY_EDITOR
                _callbackForRegister = callback;
                _registeredSize = 0;
                _registerSize = 1;
#else
                isLoading = false;
                callback();
#endif
            }
        }

        private static int listSize;
        private static string[] idList;
        private static byte[][] binaryDataList;
        private static uint[] sizeList;
        private static uint[] loopStartPointList;
        private static uint[] loopLengthList;

        public static void RegisterSounds(HarmoniaRegister harmoniaRegister, Action callback = null)
        {
            if(isLoading)
                return;
            isLoading = true;
            
            listSize = harmoniaRegister.Count;

            idList = new string[listSize];
            binaryDataList = new byte[listSize][];
            sizeList = new uint[listSize];
            loopStartPointList = new uint[listSize];
            loopLengthList = new uint[listSize];

            for (int i = 0; i < listSize; i++)
            {
                idList[i] = harmoniaRegister.IdList[i];
                binaryDataList[i] = harmoniaRegister.BinaryDataList[i];
                sizeList[i] = harmoniaRegister.SizeList[i];
                loopStartPointList[i] = harmoniaRegister.LoopStartPointList[i];
                loopLengthList[i] = harmoniaRegister.LoopLengthList[i];
            }

            // 実行
#if UNITY_WEBGL && !UNITY_EDITOR
            _callbackForRegister = callback;
            _registeredSize = 0;
            _registerSize = listSize;

            decodeSound(0);
#else
            for(int i = 0; i < listSize; i++)
            {
                harmonia_register_sound(idList[i], binaryDataList[i], sizeList[i], loopStartPointList[i], loopLengthList[i]);     
            }
#endif

            

            // コールバック処理
            if(callback != null)
            {

#if UNITY_WEBGL && !UNITY_EDITOR

#else
                isLoading = false;
                callback();
#endif
            }
        }

        private static void decodeSound(int index)
        {
#if UNITY_WEBGL && !UNITY_EDITOR
            harmonia_register_sound(idList[index], binaryDataList[index], sizeList[index], loopStartPointList[index], loopLengthList[index], _CallbackForRegisterComplete);
#endif
        }

        [MonoPInvokeCallback(typeof(Action))]
        public static void _CallbackForRegisterComplete()
        {
            Debug.Log("_CallbackForRegisterComplete");
            _registeredSize++;
            if (_registeredSize >= _registerSize)
            {
                if (_callbackForRegister != null)
                {
                    _callbackForRegister();
                }
                
                _callbackForRegister = null;
                _registeredSize = 0;
                _registerSize = 0;
                isLoading = false;
            }
            else
            {
                decodeSound(_registeredSize);
            }
        }


        public static void UnregisterSound(string id)
        {
            harmonia_unregister_sound(id);
        }

        public static void Pause()
        {
            harmonia_pause_all();
        }

        public static void Resume()
        {
            harmonia_resume_all();
        }

        public static void Stop()
        {
            harmonia_stop_all();
        }


        public static void Mute()
        {
            harmonia_mute_all();
        }

        public static void Unmute()
        {
            harmonia_unmute_all();
        }

        public static void StartCaptureEvents()
        {
            harmonia_start_capture_events();
        }

        public static HarmoniaEventDataList GetCaptureEvents()
        {
            string jsonText;

#if UNITY_WEBGL && !UNITY_EDITOR
            jsonText = harmonia_get_capture_events();
#else
            int size = 0;
            IntPtr result = harmonia_get_capture_events_with_size(out size);

            if(result == IntPtr.Zero || size == 0)
            {
                return null;
            }

            byte[] buffer = new byte[(int)size];
            Marshal.Copy(result, buffer, 0, (int)size);

            jsonText = System.Text.Encoding.UTF8.GetString(buffer);
#endif


            HarmoniaEventDataList dataList = JsonUtility.FromJson<HarmoniaEventDataList>(jsonText);

            return dataList;
        }

        public static void StopCaptureEvents()
        {
            harmonia_stop_capture_events();
        }


        public static void StartCaptureErrors()
        {
            harmonia_start_capture_errors();
        }

        public static HarmoniaErrorDataList GetCaptureErrors()
        {
            string jsonText;
#if UNITY_WEBGL && !UNITY_EDITOR
            jsonText = harmonia_get_capture_errors();
#else
            int size = 0;
            IntPtr result = harmonia_get_capture_errors_with_size(out size);

            if(result == IntPtr.Zero || size == 0)
            {
                return null;
            }

            byte[] buffer = new byte[(int)size];
            Marshal.Copy(result, buffer, 0, (int)size);
            jsonText = System.Text.Encoding.UTF8.GetString(buffer);
#endif

            HarmoniaErrorDataList dataList = JsonUtility.FromJson<HarmoniaErrorDataList>(jsonText);

            return dataList;
        }

        public static void StopCaptureErrors()
        {
            harmonia_stop_capture_errors();
        }

        public static HarmoniaSoundChannel Play(string registeredId, string soundId = "", HarmoniaGroup targetGruop = null)
        {
            Debug.Log("harmonia_sound_play");
            harmonia_sound_play(registeredId, soundId, (targetGruop != null) ? targetGruop.id : "");

            return new HarmoniaSoundChannel(soundId);
        }


        public static HarmoniaGroup CreateGroup(string groupId, HarmoniaGroup parentGroup = null, int maxPolyphony = -1)
        {
            HarmoniaGroup group = new HarmoniaGroup(groupId, parentGroup, maxPolyphony);
            return group;
        }
    }

}
