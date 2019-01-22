/**
*  Harmonia
*
*  Copyright (c) 2018 Hiiragi
*
*  This software is released under the MIT License.
*  http://opensource.org/licenses/mit-license.php
*/

#include "AbstractEffectCommand.h"


AbstractEffectCommand::AbstractEffectCommand()
{
}


AbstractEffectCommand::~AbstractEffectCommand()
{
}


void AbstractEffectCommand::execute(int* combinedBuffer, int size)
{
}

void AbstractEffectCommand::set_enabled(bool enabled)
{
	_enabled = enabled;
}

bool AbstractEffectCommand::get_enabled()
{
	return _enabled;
}