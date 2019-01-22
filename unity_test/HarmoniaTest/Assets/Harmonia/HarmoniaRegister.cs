using System.Collections.Generic;
using UnityEngine;

namespace jp.Hiiragi
{
    public class HarmoniaRegister
    {

        public int Count { get; private set; }

        public List<string> IdList { get; private set; }

        public List<byte[]> BinaryDataList { get; private set; }

        public List<uint> SizeList { get; private set; }

        public List<uint> LoopStartPointList { get; private set; }

        public List<uint> LoopLengthList { get; private set; }

        public HarmoniaRegister()
        {
            Count = 0;
            IdList = new List<string>();
            BinaryDataList = new List<byte[]>();
            SizeList = new List<uint>();
            LoopStartPointList = new List<uint>();
            LoopLengthList = new List<uint>();
        }

        public void Add(string id, byte[] binaryData, uint loopStartPoint = 0, uint loopLength = 0)
        {
            Count++;
            this.IdList.Add(id);
            this.BinaryDataList.Add(binaryData);
			this.SizeList.Add((uint)binaryData.Length);
            this.LoopStartPointList.Add(loopStartPoint);
            this.LoopLengthList.Add(loopLength);
        }

    }

}

