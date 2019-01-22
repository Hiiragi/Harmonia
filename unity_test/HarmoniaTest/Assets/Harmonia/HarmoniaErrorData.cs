using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace jp.Hiiragi
{
    [Serializable]
    public class HarmoniaErrorDataList
    {
        [SerializeField]
        private List<HarmoniaErrorData> errors;

        public List<HarmoniaErrorData> Errors
        {
            get
            {
                return errors;
            }
        }
    }

    [Serializable]
    public class HarmoniaErrorData
    {
        [SerializeField]
        private int type;

        public int Type
        {
            get
            {
                return type;
            }
        }
    }
}
