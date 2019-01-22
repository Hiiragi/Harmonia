using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UnityEngine;



namespace jp.Hiiragi
{
    public class HarmoniaGroup
    {




#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_create_group(string groupId, string parentGroupId = "", int maxPolyphony = 0);


#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_delete_group(string groupId);



#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_group_pause(string groupId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_group_resume(string groupId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_group_stop(string groupId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_group_mute(string groupId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_group_unmute(string groupId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern float harmonia_get_group_volume(string groupId);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_set_group_volume(string groupId, float volume);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern void harmonia_set_ducker(string triggerGroupId, string targetGroupId, float ratio, float attackTimeByMS = 0, float releaseTimeByMS = 0);

#if(UNITY_IOS || UNITY_WEBGL) && !UNITY_EDITOR
    [DllImport("__Internal")]
#else
        [DllImport("Harmonia")]
#endif
        private static extern uint harmonia_get_group_status(string targetGroupId);








        public string id { get; private set; }

        public HarmoniaGroup parent { get; private set; }



        public float Volume
        {
            get
            {
                return harmonia_get_group_volume(id);
            }
            set
            {
                harmonia_set_group_volume(id, value);
            }
        }





        public HarmoniaGroup(string groupId, HarmoniaGroup parentGroup = null, int maxPolyphony = -1)
        {
            id = groupId;
            parent = parentGroup;
            
            harmonia_create_group(groupId, (parentGroup != null) ? parentGroup.id : "", maxPolyphony);
        }


        public void Pause()
        {
            harmonia_group_pause(id);
        }

        public void Resume()
        {
            harmonia_group_resume(id);
        }

        public void Stop()
        {
            harmonia_group_stop(id);
        }

        public void Mute()
        {
            harmonia_group_mute(id);
        }

        public void Unmute()
        {
            harmonia_group_unmute(id);
        }

        public void SetDucker(HarmoniaGroup triggerGroup, float ratio, float attackTimeByMS = 0, float releaseTimeByMS = 0)
        {
            harmonia_set_ducker(triggerGroup.id, id, ratio, attackTimeByMS, releaseTimeByMS);
        }
    }

}
