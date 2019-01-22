using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace jp.Hiiragi
{
    [Serializable]
    public class HarmoniaEventDataList
    {
        [SerializeField]
        private List<HarmoniaEventData> events;

        public List<HarmoniaEventData> Events
        {
            get
            {
                return events;
            }
        }
    }

    [Serializable]
    public class HarmoniaEventData
    {
        [SerializeField]
        private string rid;
        [SerializeField]
        private string sid;
        [SerializeField]
        private int type;

        public string Sid
        {
            get
            {
                return sid;
            }
        }

        public string Rid
        {
            get
            {
                return rid;
            }
        }
        
        public int Type
        {
            get
            {
                return type;
            }
        }
    }

}
