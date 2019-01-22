using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using jp.Hiiragi;
using System;

public class HarmoniaBehaviour : MonoBehaviour {

    [SerializeField]
    private TextAsset _SampleBgm;

    [SerializeField]
    private TextAsset _SampleSe;

    [SerializeField]
    private Button _PlaySeButton;


    [SerializeField]
    private Slider _MasterVolumeSlider;
    [SerializeField]
    private Slider _BgmGroupVolumeSlider;
    [SerializeField]
    private Slider _SeGroupVolumeSlider;
    [SerializeField]
    private Slider _BgmVolumeSlider;

    [SerializeField]
    private Text _MasterVolumeText;
    [SerializeField]
    private Text _BgmGroupVolumeText;
    [SerializeField]
    private Text _SeGroupVolumeText;
    [SerializeField]
    private Text _BgmVolumeText;

    [SerializeField]
    private Toggle _MasterMuteToggle;
    [SerializeField]
    private Toggle _BgmGroupMuteToggle;
    [SerializeField]
    private Toggle _SeGroupMuteToggle;
    [SerializeField]
    private Toggle _BgmMuteToggle;

    [SerializeField]
    private Button _MasterPauseButton;
    [SerializeField]
    private Button _BgmGroupPauseButton;
    [SerializeField]
    private Button _SeGroupPauseButton;
    [SerializeField]
    private Button _BgmPauseButton;

    [SerializeField]
    private Button _MasterResumeButton;
    [SerializeField]
    private Button _BgmGroupResumeButton;
    [SerializeField]
    private Button _SeGroupResumeButton;
    [SerializeField]
    private Button _BgmResumeButton;


    [SerializeField]
    private Text _BgmCurrentPositionText;

    private HarmoniaGroup _bgmGroup;
    private HarmoniaGroup _seGroup;

    private HarmoniaSoundChannel _bgmSoundChannel;

    private bool isReady = false;

    
    void Start () {
        
        Harmonia.Initialize(2048);

        Harmonia.StartCaptureEvents();
        Harmonia.StartCaptureErrors();

        HarmoniaRegister harmoniaRegister = new HarmoniaRegister();
        
        byte[] bgm = _SampleBgm.bytes;
        harmoniaRegister.Add("bgm_sample", bgm, 269128, 2116816);
        
        byte[] se = _SampleSe.bytes;
        harmoniaRegister.Add("se_sample", se);
        
        Harmonia.RegisterSounds(harmoniaRegister, ()=>
        {
            
            Debug.Log("RegisterSounds completed");

            // group
            _bgmGroup = Harmonia.CreateGroup("BgmGroup");
            _seGroup = Harmonia.CreateGroup("SeGroup");
            
            // duck
            _bgmGroup.SetDucker(_seGroup, 0.5f, 100, 500);
            
            // Master Volume
            Harmonia.MasterVolume = 0.5f;
            _MasterVolumeSlider.value = Harmonia.MasterVolume;
            
            // Play BGM
            _bgmSoundChannel = Harmonia.Play("bgm_sample", "bgm", _bgmGroup);
            
            _PlaySeButton.onClick.AddListener(OnClickPlaySeButton);

            _MasterVolumeSlider.onValueChanged.AddListener(OnValueChangeMasterVolumeSlider);
            _BgmGroupVolumeSlider.onValueChanged.AddListener(OnValueChangeBgmGroupVolumeSlider);
            _SeGroupVolumeSlider.onValueChanged.AddListener(OnValueChangeSeGroupVolumeSlider);
            _BgmVolumeSlider.onValueChanged.AddListener(OnValueChangeBgmVolumeSlider);

            isReady = true;

            _MasterMuteToggle.onValueChanged.AddListener(OnValueChangeMasterMuteToggle);
            _BgmGroupMuteToggle.onValueChanged.AddListener(OnValueChangeBgmGroupMuteToggle);
            _SeGroupMuteToggle.onValueChanged.AddListener(OnValueChangeSeGroupMuteToggle);
            _BgmMuteToggle.onValueChanged.AddListener(OnValueChangeBgmMuteToggle);


            _MasterPauseButton.onClick.AddListener(OnClickMasterPauseButton);
            _BgmGroupPauseButton.onClick.AddListener(OnClickBgmGroupPauseButton);
            _SeGroupPauseButton.onClick.AddListener(OnClickSeGroupPauseButton);
            _BgmPauseButton.onClick.AddListener(OnClickBgmPauseButton);

            _MasterResumeButton.onClick.AddListener(OnClickMasterResumeButton);
            _BgmGroupResumeButton.onClick.AddListener(OnClickBgmGroupResumeButton);
            _SeGroupResumeButton.onClick.AddListener(OnClickSeGroupResumeButton);
            _BgmResumeButton.onClick.AddListener(OnClickBgmResumeButton);
        });

    }




    // Update is called once per frame
    void Update ()
    {
        HarmoniaEventDataList eventList = Harmonia.GetCaptureEvents();
        
        if (eventList != null)
        {
            List<HarmoniaEventData> events = eventList.Events;
            for (int i = 0; i < events.Count; i++)
            {
                HarmoniaEventData eventData = events[i];
                Debug.Log("=== event ======= rid = " + eventData.Rid + " , sid " + eventData.Sid + " , type " + eventData.Type);
            }
        }

        HarmoniaErrorDataList errorList = Harmonia.GetCaptureErrors();
       
        if(errorList != null)
        {
            List<HarmoniaErrorData> errors = errorList.Errors;

            for(int i = 0; i < errors.Count; i++)
            {
                HarmoniaErrorData errorData = errors[i];
                Debug.Log("=== error ======= type " + errorData.Type);
            }
        }
        
        if (isReady)
        {
            _MasterVolumeText.text = Harmonia.MasterVolume.ToString();
            _BgmGroupVolumeText.text = _bgmGroup.Volume.ToString();
            _SeGroupVolumeText.text = _seGroup.Volume.ToString();
            _BgmVolumeText.text = _bgmSoundChannel.Volume.ToString();

            _BgmCurrentPositionText.text = _bgmSoundChannel.CurrentPosition.ToString();
        }

    }

    private void OnClickPlaySeButton()
    {
        Harmonia.Play("se_sample", "", _seGroup);
    }

    private void OnValueChangeMasterVolumeSlider(float value)
    {
        Harmonia.MasterVolume = value;
    }

    private void OnValueChangeBgmGroupVolumeSlider(float value)
    {
        _bgmGroup.Volume = value;
    }

    private void OnValueChangeSeGroupVolumeSlider(float value)
    {
        _seGroup.Volume = value;
    }

    private void OnValueChangeBgmVolumeSlider(float value)
    {
        _bgmSoundChannel.Volume = value;
    }

    private void OnValueChangeMasterMuteToggle(bool value)
    {
        if (value)
        {
            Harmonia.Mute();
        }
        else
        {
            Harmonia.Unmute();
        }
    }

    private void OnValueChangeBgmGroupMuteToggle(bool value)
    {
        if (value)
        {
            _bgmGroup.Mute();
        }
        else
        {
            _bgmGroup.Unmute();
        }
    }

    private void OnValueChangeSeGroupMuteToggle(bool value)
    {
        if (value)
        {
            _seGroup.Mute();
        }
        else
        {
            _seGroup.Unmute();
        }
    }

    private void OnValueChangeBgmMuteToggle(bool value)
    {
        if (value)
        {
            _bgmSoundChannel.Mute();
        }
        else
        {
            _bgmSoundChannel.Unmute();
        }
    }

    private void OnClickMasterPauseButton()
    {
        Harmonia.Pause();
    }

    private void OnClickBgmGroupPauseButton()
    {
        _bgmGroup.Pause();
    }

    private void OnClickSeGroupPauseButton()
    {
        _seGroup.Pause();
    }

    private void OnClickBgmPauseButton()
    {
        _bgmSoundChannel.Pause();
    }

    private void OnClickMasterResumeButton()
    {
        Harmonia.Resume();
    }

    private void OnClickBgmGroupResumeButton()
    {
        _bgmGroup.Resume();
    }

    private void OnClickSeGroupResumeButton()
    {
        _seGroup.Resume();
    }

    private void OnClickBgmResumeButton()
    {
        _bgmSoundChannel.Resume();
    }

    private void OnDestroy()
    {
        Harmonia.Finaliz();
    }
}
