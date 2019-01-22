/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "DuckerCommand.h"
#include "Harmonia.h"

DuckerCommand::DuckerCommand(SoundGroup* triggerGroup, float ratio, float attackTimeByMS, float releaseTimeByMS)
{
	_triggerGroup = triggerGroup;
	_ratio = ratio;
	_currentRatio = 1;
	_diffRatioForAttackTimeBySample = (1.0 - ratio) / (44100 * attackTimeByMS / 1000);
	_diffRatioForReleaseTimeBySample = (1.0 - ratio) / (44100 * releaseTimeByMS / 1000);
}


DuckerCommand::~DuckerCommand()
{
}


void DuckerCommand::execute(int* combinedBuffer, int size)
{
	if (_triggerGroup->get_current_polyphony() > 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (_currentRatio > _ratio)
			{
				_currentRatio -= _diffRatioForAttackTimeBySample;
				if (_currentRatio < _ratio)
				{
					_currentRatio = _ratio;
				}
			}
			combinedBuffer[i] = (int)(combinedBuffer[i] * _currentRatio);

		}
	}
	else
	{
		for (int i = 0; i < size; i++)
		{
			if (_currentRatio < 1)
			{
				_currentRatio += _diffRatioForReleaseTimeBySample;
				if (_currentRatio > 1)
				{
					_currentRatio = 1;
				}
			}

			combinedBuffer[i] = (int)(combinedBuffer[i] * _currentRatio);
		}
	}
}
