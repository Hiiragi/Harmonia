/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#pragma once

#include "AbstractEffectCommand.h"
#include "SoundGroup.h"

class DuckerCommand : public AbstractEffectCommand
{
public:
	DuckerCommand(SoundGroup* triggerGroup, float ratio, float attackTimeByMS = 0, float releaseTimeByMS = 0);
	virtual ~DuckerCommand();

	void execute(int* combinedBuffer, int size);
	
private:
	SoundGroup* _triggerGroup;
	float _ratio;
	double _currentRatio;
	double _diffRatioForAttackTimeBySample;
	double _diffRatioForReleaseTimeBySample;
};

